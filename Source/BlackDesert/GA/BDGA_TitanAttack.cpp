// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_TitanAttack.h"
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

UBDGA_TitanAttack::UBDGA_TitanAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}


void UBDGA_TitanAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
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
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBDGA_TitanAttack::OnMontageEnded);

    // 애니메이션 노티파이 콜백 바인딩 - 특정 노티파이 시점에서 호출될 함수
    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBDGA_TitanAttack::OnAttackNotify);

    // 몽타주 재생
    AnimInstance->Montage_Play(AttackMontage);
    UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Played: %s"), *AttackMontage->GetName());

    ExecuteAttack();
}

void UBDGA_TitanAttack::OnAttackNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    
}

void UBDGA_TitanAttack::ExecuteAttack()
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
            if (World)
            {
                // HitEffect를 0.3초 후에 실행
                FTimerHandle TimerHandle;
                FVector EffectLocation = Character->GetActorLocation();
                FRotator EffectRotation = Character->GetActorRotation();

                // 타이머 설정: 0.3초 후에 PlayDelayedHitEffect 함수 실행
                World->GetTimerManager().SetTimer(
                    TimerHandle,
                    FTimerDelegate::CreateUObject(this, &UBDGA_TitanAttack::PlayDelayedHitEffect, EffectLocation, EffectRotation),
                    0.3f,
                    false
                );
            }
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

void UBDGA_TitanAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
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
                AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UBDGA_TitanAttack::OnAttackNotify);
                AnimInstance->OnMontageEnded.RemoveDynamic(this, &UBDGA_TitanAttack::OnMontageEnded);
            }
        }

        AlreadyHitActors.Empty();

        // 어빌리티 종료
        K2_EndAbility();
    }
}


void UBDGA_TitanAttack::PlayDelayedHitEffect(FVector Location, FRotator Rotation)
{
    UWorld* World = GetWorld();
    if (World && HitEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World,
            HitEffect,
            Location,
            Rotation
        );
    }
}