// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_TitanPrimary.h"
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


UBDGA_TitanPrimary::UBDGA_TitanPrimary()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBDGA_TitanPrimary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

    // 노티파이 콜백 바인딩 - 몽타주가 끝날 때 호출될 함수
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBDGA_TitanPrimary::OnMontageEnded);

    // 애니메이션 노티파이 콜백 바인딩 - 특정 노티파이 시점에서 호출될 함수
    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBDGA_TitanPrimary::OnAttackNotify);

    // 몽타주 재생
    AnimInstance->Montage_Play(AttackMontage);
    UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Played: %s"), *AttackMontage->GetName());

    ApplySpeedBoost();
    ExecuteAttack();
}

void UBDGA_TitanPrimary::OnAttackNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
}

void UBDGA_TitanPrimary::ExecuteAttack()
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

void UBDGA_TitanPrimary::ApplySpeedBoost()
{
    ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
    if (!Character) return;

    // 캐릭터 이동 컴포넌트 가져오기
    UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
    if (!MovementComponent) return;

    // 기존 타이머가 활성화되어 있으면 제거
    FTimerManager& TimerManager = GetWorld()->GetTimerManager();
    if (TimerManager.IsTimerActive(SpeedBoostTimerHandle))
    {
        TimerManager.ClearTimer(SpeedBoostTimerHandle);
    }

    // 원래 이동 속도 저장
    OriginalMovementSpeed = MovementComponent->MaxWalkSpeed;

    // 이동 속도 증가 적용
    MovementComponent->MaxWalkSpeed = OriginalMovementSpeed * SpeedMultiplier;

    UE_LOG(LogTemp, Log, TEXT("movement speed : %f -> %f"), OriginalMovementSpeed, MovementComponent->MaxWalkSpeed);

    // 지정된 시간 후에 속도 원래대로 복구하는 타이머 설정
    TimerManager.SetTimer(SpeedBoostTimerHandle, this, &UBDGA_TitanPrimary::ResetSpeed, SpeedBoostDuration, false);
}

void UBDGA_TitanPrimary::ResetSpeed()
{
    ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
    if (!Character) return;

    // 캐릭터 이동 컴포넌트 가져오기
    UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
    if (!MovementComponent) return;

    // 원래 이동 속도로 복구
    MovementComponent->MaxWalkSpeed = OriginalMovementSpeed;

    UE_LOG(LogTemp, Log, TEXT("change to origin character speed : %f"), OriginalMovementSpeed);

    // 어빌리티 종료
    K2_EndAbility();
}

void UBDGA_TitanPrimary::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
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
                AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UBDGA_TitanPrimary::OnAttackNotify);
                AnimInstance->OnMontageEnded.RemoveDynamic(this, &UBDGA_TitanPrimary::OnMontageEnded);
            }
        }

        if (bInterrupted)
        {
            FTimerManager& TimerManager = GetWorld()->GetTimerManager();
            if (TimerManager.IsTimerActive(SpeedBoostTimerHandle))
            {
                TimerManager.ClearTimer(SpeedBoostTimerHandle);
                ResetSpeed();
            }
        }

        AlreadyHitActors.Empty();

        
    }
}
