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

    // 1. Casting Effect ����
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


    // ��Ƽ���� �ݹ� ���ε� - ��Ÿ�ְ� ���� �� ȣ��� �Լ�
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBDGA_FletcherTertiary::OnMontageEnded);

    // �ִϸ��̼� ��Ƽ���� �ݹ� ���ε� - Ư�� ��Ƽ���� �������� ȣ��� �Լ�
    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBDGA_FletcherTertiary::OnAttackNotify);

    // 2. ��Ÿ�� ���
    AnimInstance->Montage_Play(AttackMontage, 2.f);
    UE_LOG(LogTemp, Log, TEXT("Titan Attack Montage Played: %s"), *AttackMontage->GetName());

    // ExecuteAttack() ȣ�� ���� (��Ÿ�� ���� �� ȣ��� ����)
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

            // �ڱ� �ڽ��̰ų� �̹� ���� �� ������ ����
            if (!HitActor || HitActor == Character || AlreadyHitActors.Contains(HitActor))
                continue;

            ABDNeutralMonsterBase* Monster = Cast<ABDNeutralMonsterBase>(HitActor);
            if (!Monster)
                continue;

            AlreadyHitActors.Add(HitActor); // �ߺ� ������ ����

            UE_LOG(LogTemp, Log, TEXT("Titan Attack Hit Actor: %s"), *HitActor->GetName());

            if (HitEffect)
            {
                // ImpactPoint���� Z������ -2 offset ����
                FVector EffectLocation = Hit.ImpactPoint;
                EffectLocation.Z -= 2.0f;  // Z������ -2 offset ����

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

        // 3. ��Ÿ�ְ� ������ Execute ����
        ExecuteAttack();

        // �ݹ� �ڵ� ����
        ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
        if (Character)
        {
            UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                // ���ε��� ��������Ʈ ����
                AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UBDGA_FletcherTertiary::OnAttackNotify);
                AnimInstance->OnMontageEnded.RemoveDynamic(this, &UBDGA_FletcherTertiary::OnMontageEnded);
            }
        }

        AlreadyHitActors.Empty();

        // �����Ƽ ����
        K2_EndAbility();
    }
}


void UBDGA_FletcherTertiary::PerformDash(ACharacter* Character)
{
    if (!Character) return;

    // ĳ���Ͱ� �ٶ󺸴� ���� ��������
    FVector ForwardVector = Character->GetActorForwardVector();

    float offset = 8.f;

    // �̵��� ���� ��� (���� ���� * �뽬 �Ÿ�) // �ڷ� �̵� 
    FVector DashVector = -ForwardVector * DashDistance * offset;

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