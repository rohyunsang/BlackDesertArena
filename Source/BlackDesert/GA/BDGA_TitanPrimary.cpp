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

    // ��Ƽ���� �ݹ� ���ε� - ��Ÿ�ְ� ���� �� ȣ��� �Լ�
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBDGA_TitanPrimary::OnMontageEnded);

    // �ִϸ��̼� ��Ƽ���� �ݹ� ���ε� - Ư�� ��Ƽ���� �������� ȣ��� �Լ�
    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBDGA_TitanPrimary::OnAttackNotify);

    // ��Ÿ�� ���
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
        {},  // �� �� �迭�� ó��
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

            // �ڱ� �ڽ��̰ų� �̹� ���� �� ������ ����
            if (!HitActor || HitActor == Character || AlreadyHitActors.Contains(HitActor))
                continue;

            AlreadyHitActors.Add(HitActor); // �ߺ� ������ ����

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

    // ĳ���� �̵� ������Ʈ ��������
    UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
    if (!MovementComponent) return;

    // ���� Ÿ�̸Ӱ� Ȱ��ȭ�Ǿ� ������ ����
    FTimerManager& TimerManager = GetWorld()->GetTimerManager();
    if (TimerManager.IsTimerActive(SpeedBoostTimerHandle))
    {
        TimerManager.ClearTimer(SpeedBoostTimerHandle);
    }

    // ���� �̵� �ӵ� ����
    OriginalMovementSpeed = MovementComponent->MaxWalkSpeed;

    // �̵� �ӵ� ���� ����
    MovementComponent->MaxWalkSpeed = OriginalMovementSpeed * SpeedMultiplier;

    UE_LOG(LogTemp, Log, TEXT("movement speed : %f -> %f"), OriginalMovementSpeed, MovementComponent->MaxWalkSpeed);

    // ������ �ð� �Ŀ� �ӵ� ������� �����ϴ� Ÿ�̸� ����
    TimerManager.SetTimer(SpeedBoostTimerHandle, this, &UBDGA_TitanPrimary::ResetSpeed, SpeedBoostDuration, false);
}

void UBDGA_TitanPrimary::ResetSpeed()
{
    ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
    if (!Character) return;

    // ĳ���� �̵� ������Ʈ ��������
    UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
    if (!MovementComponent) return;

    // ���� �̵� �ӵ��� ����
    MovementComponent->MaxWalkSpeed = OriginalMovementSpeed;

    UE_LOG(LogTemp, Log, TEXT("change to origin character speed : %f"), OriginalMovementSpeed);

    // �����Ƽ ����
    K2_EndAbility();
}

void UBDGA_TitanPrimary::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == AttackMontage)
    {
        UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Ended"));

        // �ݹ� �ڵ� ����
        ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
        if (Character)
        {
            UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                // ���ε��� ��������Ʈ ����
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
