// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_TitanSecondary.h"
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


UBDGA_TitanSecondary::UBDGA_TitanSecondary()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBDGA_TitanSecondary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

    OwningCharacter = Character;


    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    


    // 노티파이 콜백 바인딩 - 몽타주가 끝날 때 호출될 함수
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBDGA_TitanSecondary::OnMontageEnded);

    // 애니메이션 노티파이 콜백 바인딩 - 특정 노티파이 시점에서 호출될 함수
    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBDGA_TitanSecondary::OnAttackNotify);


    if (CastingEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            CastingEffect,
            Character->GetActorLocation()
        );
    }

    // 몽타주 재생
    AnimInstance->Montage_Play(AttackMontage);
    UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Played: %s"), *AttackMontage->GetName());

    // 대시와 공격을 0.7초 후에 실행하도록 타이머 설정
    UWorld* World = GetWorld();
    if (World)
    {
        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            &UBDGA_TitanSecondary::DelayedDashAndAttack,
            0.7f,
            false
        );
    }
}

void UBDGA_TitanSecondary::OnAttackNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
}

void UBDGA_TitanSecondary::ExecuteAttack()
{
    UE_LOG(LogTemp, Log, TEXT("Titan ExecuteAttack()"));

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

        if (HitEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World,
                HitEffect,
                Character->GetActorLocation(),  // 플레이어 위치
                Character->GetActorRotation()   // 플레이어 회전값
            );
        }


        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();

            // 자기 자신이거나 이미 맞은 적 있으면 무시
            if (!HitActor || HitActor == Character || AlreadyHitActors.Contains(HitActor))
                continue;

            AlreadyHitActors.Add(HitActor); // 중복 방지용 저장

            UE_LOG(LogTemp, Log, TEXT("Titan Attack Hit Actor: %s"), *HitActor->GetName());

            

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



void UBDGA_TitanSecondary::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == AttackMontage)
    {
        UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Ended"));

        // 콜백 핸들 정리
        ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
        if (Character)
        {
            UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                // 바인딩한 델리게이트 제거
                AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UBDGA_TitanSecondary::OnAttackNotify);
                AnimInstance->OnMontageEnded.RemoveDynamic(this, &UBDGA_TitanSecondary::OnMontageEnded);
            }
        }

        AlreadyHitActors.Empty();

        if (SkillEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                SkillEffect,
                Character->GetActorLocation()
            );
        }

        // 어빌리티 종료
        K2_EndAbility();
    }
}

void UBDGA_TitanSecondary::PerformDash(ACharacter* Character)
{
    if (!Character) return;

    // 캐릭터가 바라보는 방향 가져오기
    FVector ForwardVector = Character->GetActorForwardVector();

    float offset = 8.f;

    // 이동할 벡터 계산 (전방 벡터 * 대쉬 거리)
    FVector DashVector = ForwardVector * DashDistance * offset;

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

void UBDGA_TitanSecondary::DelayedDashAndAttack()
{
    if (OwningCharacter)
    {
        // 0.7초 후에 대시 실행
        PerformDash(OwningCharacter);

        // 대시 후 즉시 공격 실행
        ExecuteAttack();
    }
}