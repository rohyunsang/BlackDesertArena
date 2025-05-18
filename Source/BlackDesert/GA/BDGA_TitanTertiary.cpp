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

    // ù��° ���ݽ� ĳ���� ����Ʈ ����
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

    // �÷��̾ �ٶ󺸴� �������� �뽬 �̵�
    PerformDash(Character);


    // ��Ƽ���� �ݹ� ���ε� - ��Ÿ�ְ� ���� �� ȣ��� �Լ�
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBDGA_TitanTertiary::OnMontageEnded);

    // �ִϸ��̼� ��Ƽ���� �ݹ� ���ε� - Ư�� ��Ƽ���� �������� ȣ��� �Լ�
    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBDGA_TitanTertiary::OnAttackNotify);

    // ��Ÿ�� ���
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

void UBDGA_TitanTertiary::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // �� �ڼ��� �α� �߰�
    UE_LOG(LogTemp, Warning, TEXT("Titan Attack Montage Ended: %s, CurrentAttackCount: %d, Interrupted: %d"),
        *Montage->GetName(), CurrentAttackCount, bInterrupted ? 1 : 0);

    // ��Ÿ�� �� ����
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

    // �ߴܵǾ����� �����Ƽ ����
    if (bInterrupted)
    {
        // ���� �ڵ�...
    }

    // ��Ÿ�� ��Ī Ȯ��
    bool bIsFirstMontage = (Montage == AttackMontage);
    UE_LOG(LogTemp, Warning, TEXT("Is First Montage Match: %d, CurrentAttackCount: %d"),
        bIsFirstMontage ? 1 : 0, CurrentAttackCount);

    // ù ��° ������ ������ �� ��° ���� ����
    if (Montage == AttackMontage && CurrentAttackCount == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting second attack!"));
        CurrentAttackCount++;
        PerformSecondAttack();
    }
    // �� ��° ������ ������ �����Ƽ ����
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
        // �ݹ� �ڵ� ����...
    }
    else
    {
        // �� �κе� �߰�
        UE_LOG(LogTemp, Error, TEXT("UNEXPECTED CASE: Montage doesn't match either attack montage!"));
    }
}

void UBDGA_TitanTertiary::PerformDash(ACharacter* Character)
{
    if (!Character) return;

    // ĳ���Ͱ� �ٶ󺸴� ���� ��������
    FVector ForwardVector = Character->GetActorForwardVector();

    float offset = 5.f;

    // �̵��� ���� ��� (���� ���� * �뽬 �Ÿ�)
    FVector DashVector = ForwardVector * DashDistance * offset;

    // Z�� ������ ���� (������ �������� �ʵ���)
    if (bPreventFalling)
    {
        DashVector.Z = 0.0f;
    }

    // ĳ���� �߻�(�뽬)
    // �� ��° �Ű������� true�� ���� �ӵ��� �����, false�� ���� �ӵ��� �߰�
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


    // �� ��° ��Ÿ�� ���
    float PlayRate = AnimInstance->Montage_Play(SecondAttackMontage);
    UE_LOG(LogTemp, Warning, TEXT("Titan Second Attack Montage Played: %s, PlayRate: %f"),
        *SecondAttackMontage->GetName(), PlayRate);

    // �տ��� ����� �迭 �ʱ�ȭ
    AlreadyHitActors.Empty();

    // �� ��° ���� ���� ����
    ExecuteAttack();

}