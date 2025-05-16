// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_FletcherTertiary.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Actor/BDArcherProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Monster/BDNeutralMonsterBase.h"


UBDGA_FletcherTertiary::UBDGA_FletcherTertiary()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}
void UBDGA_FletcherTertiary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    UE_LOG(LogTemp, Warning, TEXT("BD_LOG GA_TitanAttack::ActivateAbility called!"));

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character || !AttackMontage)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 1. Casting Effect 실행
    if (CastingEffect)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World,
                CastingEffect,
                Character->GetActorLocation(),
                Character->GetActorRotation()
            );
        }
    }
    
    PerformDash(Character);


    // 노티파이 콜백 바인딩 - 몽타주가 끝날 때 호출될 함수
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBDGA_FletcherTertiary::OnMontageEnded);

    // 애니메이션 노티파이 콜백 바인딩 - 특정 노티파이 시점에서 호출될 함수
    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBDGA_FletcherTertiary::OnAttackNotify);

    // 2. 몽타주 재생
    AnimInstance->Montage_Play(AttackMontage, 2.f);
    UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Played: %s"), *AttackMontage->GetName());

    // ExecuteAttack() 호출 제거 (몽타주 종료 시 호출로 변경)
}

void UBDGA_FletcherTertiary::OnAttackNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
}

void UBDGA_FletcherTertiary::ExecuteAttack()
{
    UE_LOG(LogTemp, Log, TEXT("Fletcher ExecuteAttack()"));

    ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
    if (!Character) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FVector CharacterLocation = Character->GetActorLocation();
    FVector ForwardVector = Character->GetActorForwardVector();

    FVector TraceStart = CharacterLocation;
    TraceStart.Z += Character->BaseEyeHeight / 2;
    FVector TraceEnd = TraceStart + (ForwardVector * AttackRange);

    TArray<FHitResult> HitResults;

    bool bHit = UKismetSystemLibrary::BoxTraceMulti(
        World,
        TraceStart,
        TraceEnd,
        AttackBoxExtent,
        Character->GetActorRotation(),
        UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn),
        false,
        {},  // ← 빈 배열로 처리
        EDrawDebugTrace::ForDuration,
        HitResults,
        true,
        FLinearColor::Red,
        FLinearColor::Green,
        5.0f
    );

    if (bShowDebugTrace)
    {
        DrawDebugSphere(World, TraceStart, 10.0f, 12, FColor::Blue, false, 5.0f);
        DrawDebugSphere(World, TraceEnd, 10.0f, 12, FColor::Red, false, 5.0f);
        DrawDebugLine(World, TraceStart, TraceEnd, FColor::Green, false, 5.0f);
    }

    /*
    FTransform BoxTransform(Character->GetActorRotation(), TraceStart + (TraceEnd - TraceStart) * 0.5f);
    DrawDebugBox(World, BoxTransform.GetLocation(), AttackBoxExtent, BoxTransform.GetRotation(), FColor::Purple, false, 5.0f);


    */

    if (bHit)
    {
        AController* CharacterController = Character->GetController();

        if (SkillEffect)
        {
            if (World)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    World,
                    SkillEffect,
                    Character->GetActorLocation(),
                    Character->GetActorRotation()
                );
            }
        }

        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();

            // 자기 자신이거나 이미 맞은 적 있으면 무시
            if (!HitActor || HitActor == Character || AlreadyHitActors.Contains(HitActor))
                continue;

            ABDNeutralMonsterBase* Monster = Cast<ABDNeutralMonsterBase>(HitActor);
            if (!Monster)
                continue;

            AlreadyHitActors.Add(HitActor); // 중복 방지용 저장

            UE_LOG(LogTemp, Log, TEXT("Titan Attack Hit Actor: %s"), *HitActor->GetName());

            if (HitEffect)
            {
                // ImpactPoint에서 Z축으로 -2 offset 적용
                FVector EffectLocation = Hit.ImpactPoint;
                EffectLocation.Z -= 2.0f;  // Z축으로 -2 offset 적용

                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    World,
                    HitEffect,
                    EffectLocation,
                    FRotator::ZeroRotator
                );
            }

            UGameplayStatics::ApplyDamage(
                HitActor,
                BaseDamage,
                CharacterController,
                Character,
                UDamageType::StaticClass()
            );

            UPrimitiveComponent* HitComponent = Hit.GetComponent();
            if (HitComponent && HitComponent->IsSimulatingPhysics())
            {
                FVector PushDirection = (HitActor->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
                HitComponent->AddImpulse(PushDirection * BaseDamage * 100.0f, NAME_None, true);
            }
        }
    }
}


void UBDGA_FletcherTertiary::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == AttackMontage)
    {
        UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Ended"));

        // 3. 몽타주가 끝나면 Execute 실행
        ExecuteAttack();

        // 콜백 핸들 정리
        ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
        if (Character)
        {
            UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                // 바인딩한 델리게이트 제거
                AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UBDGA_FletcherTertiary::OnAttackNotify);
                AnimInstance->OnMontageEnded.RemoveDynamic(this, &UBDGA_FletcherTertiary::OnMontageEnded);
            }
        }

        AlreadyHitActors.Empty();

        // 어빌리티 종료
        K2_EndAbility();
    }
}


void UBDGA_FletcherTertiary::PerformDash(ACharacter* Character)
{
    if (!Character) return;

    // 캐릭터가 바라보는 방향 가져오기
    FVector ForwardVector = Character->GetActorForwardVector();

    float offset = 8.f;

    // 이동할 벡터 계산 (전방 벡터 * 대쉬 거리) // 뒤로 이동 
    FVector DashVector = -ForwardVector * DashDistance * offset;

    // Z축 방향은 유지 (땅에서 떨어지지 않도록)
    if (bPreventFalling)
    {
        DashVector.Z = 0.0f;
    }

    // 캐릭터 발사(대쉬)
    // 세 번째 매개변수가 true면 현재 속도를 덮어쓰고, false면 현재 속도에 추가
    Character->LaunchCharacter(DashVector, true, !bPreventFalling);

    UE_LOG(LogTemp, Log, TEXT("Titan Dash performed: %s"), *DashVector.ToString());
}