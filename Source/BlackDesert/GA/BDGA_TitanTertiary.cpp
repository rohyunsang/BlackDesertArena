// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_TitanTertiary.h"
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


UBDGA_TitanTertiary::UBDGA_TitanTertiary()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    CurrentAttackCount = 0;
}

void UBDGA_TitanTertiary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    UE_LOG(LogTemp, Warning, TEXT("BD_LOG UBDGA_TitanTertiary::ActivateAbility called!"));

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

    // 첫번째 공격시 캐스팅 이펙트 실행
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

    // 플레이어가 바라보는 방향으로 대쉬 이동
    PerformDash(Character);


    // 노티파이 콜백 바인딩 - 몽타주가 끝날 때 호출될 함수
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBDGA_TitanTertiary::OnMontageEnded);

    // 애니메이션 노티파이 콜백 바인딩 - 특정 노티파이 시점에서 호출될 함수
    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBDGA_TitanTertiary::OnAttackNotify);

    // 몽타주 재생
    AnimInstance->Montage_Play(AttackMontage);
    UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Played: %s"), *AttackMontage->GetName());


    CurrentAttackCount = 0;
    ExecuteAttack();
}

void UBDGA_TitanTertiary::OnAttackNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
}

void UBDGA_TitanTertiary::ExecuteAttack()
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

        

        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();

            // 자기 자신이거나 이미 맞은 적 있으면 무시
            if (!HitActor || HitActor == Character || AlreadyHitActors.Contains(HitActor))
                continue;

            AlreadyHitActors.Add(HitActor); // 중복 방지용 저장

            UE_LOG(LogTemp, Log, TEXT("Titan Attack Hit Actor: %s"), *HitActor->GetName());

            if (HitEffect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    World,
                    HitEffect,
                    Hit.ImpactPoint,
                    Hit.ImpactNormal.Rotation()
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

void UBDGA_TitanTertiary::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 더 자세한 로그 추가
    UE_LOG(LogTemp, Warning, TEXT("Titan Attack Montage Ended: %s, CurrentAttackCount: %d, Interrupted: %d"),
        *Montage->GetName(), CurrentAttackCount, bInterrupted ? 1 : 0);

    // 몽타주 비교 검증
    if (AttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("AttackMontage: %s"), *AttackMontage->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AttackMontage is NULL"));
    }

    if (SecondAttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("SecondAttackMontage: %s"), *SecondAttackMontage->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SecondAttackMontage is NULL - THIS IS LIKELY THE PROBLEM!"));
    }

    // 중단되었으면 어빌리티 종료
    if (bInterrupted)
    {
        // 기존 코드...
    }

    // 몽타주 매칭 확인
    bool bIsFirstMontage = (Montage == AttackMontage);
    UE_LOG(LogTemp, Warning, TEXT("Is First Montage Match: %d, CurrentAttackCount: %d"),
        bIsFirstMontage ? 1 : 0, CurrentAttackCount);

    // 첫 번째 공격이 끝나면 두 번째 공격 시작
    if (Montage == AttackMontage && CurrentAttackCount == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting second attack!"));
        CurrentAttackCount++;
        PerformSecondAttack();
    }
    // 두 번째 공격이 끝나면 어빌리티 종료
    else if (Montage == SecondAttackMontage || CurrentAttackCount > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ending ability after second attack!"));
        
        

        // Fix this part to properly get World and Character
        ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
        UWorld* World = GetWorld();

        if (World && Character && SkillEffect2)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World,
                SkillEffect2,
                Character->GetActorLocation(),
                Character->GetActorRotation()
            );
        }

        K2_EndAbility();
        // 콜백 핸들 정리...
    }
    else
    {
        // 이 부분도 추가
        UE_LOG(LogTemp, Error, TEXT("UNEXPECTED CASE: Montage doesn't match either attack montage!"));
    }
}

void UBDGA_TitanTertiary::PerformDash(ACharacter* Character)
{
    if (!Character) return;

    // 캐릭터가 바라보는 방향 가져오기
    FVector ForwardVector = Character->GetActorForwardVector();

    float offset = 5.f;

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


void UBDGA_TitanTertiary::PerformSecondAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("PerformSecondAttack called"));

    ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
    if (!Character)
    {
        UE_LOG(LogTemp, Error, TEXT("Character is null in PerformSecondAttack"));
        K2_EndAbility();
        return;
    }

    if (SkillEffect)
    {
        UWorld* World = GetWorld();
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

    if (!SecondAttackMontage)
    {
        UE_LOG(LogTemp, Error, TEXT("SecondAttackMontage is null! Please set it in the Blueprint"));
        K2_EndAbility();
        return;
    }

    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("AnimInstance is null in PerformSecondAttack"));
        K2_EndAbility();
        return;
    }


    // 두 번째 몽타주 재생
    float PlayRate = AnimInstance->Montage_Play(SecondAttackMontage);
    UE_LOG(LogTemp, Warning, TEXT("Titan Second Attack Montage Played: %s, PlayRate: %f"),
        *SecondAttackMontage->GetName(), PlayRate);

    // 앞에서 사용한 배열 초기화
    AlreadyHitActors.Empty();

    // 두 번째 공격 판정 실행
    ExecuteAttack();

}