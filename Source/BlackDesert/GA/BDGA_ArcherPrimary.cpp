// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_ArcherPrimary.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"
#include "Actor/BDArcherProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"


UBDGA_ArcherPrimary::UBDGA_ArcherPrimary()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    // Blueprint로 만든 Projectile 클래스 참조용
    static ConstructorHelpers::FClassFinder<AActor> ProjectileBPClass(TEXT("/Game/Blueprint/Weapon/BP_ArcherProjectile.BP_ArcherProjectile_C"));

    if (ProjectileBPClass.Succeeded())
    {
        ArcherProjectileClass = ProjectileBPClass.Class;
    }
}


void UBDGA_ArcherPrimary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    UE_LOG(LogTemp, Warning, TEXT("BD_LOG GA_ArcherAttack::ActivateAbility called!"));

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (Character && ArcherProjectileClass)
    {
        GenerateFanProjectiles(Character);
    }

    // Montage 
    if (Character && AttackMontage)
    {
        if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
        {
            AnimInstance->Montage_Play(AttackMontage);
            UE_LOG(LogTemp, Log, TEXT(" Archer Attack Montage Played: %s"), *AttackMontage->GetName());
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


void UBDGA_ArcherPrimary::GenerateFanProjectiles(ACharacter* Character)
{
    const int32 ArrowCount = 5;
    const float SpreadAngle = 20.0f; // 전체 부채꼴 각도 (예: 40도)
    const float ArrowSpacingAngle = SpreadAngle / (ArrowCount - 1);

    const FVector MuzzleBase = Character->GetActorLocation() + Character->GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
    const FRotator BaseRotation = Character->GetActorRotation();

    for (int32 i = 0; i < ArrowCount; ++i)
    {
        float AngleOffset = (-SpreadAngle / 2.0f) + (i * ArrowSpacingAngle);
        FRotator ArrowRotation = BaseRotation + FRotator(0.f, AngleOffset, 0.f);
        FVector ArrowDirection = ArrowRotation.Vector();
        FVector SpawnLocation = MuzzleBase;

        // VFX
        if (ShootEffectVFX)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                ShootEffectVFX,
                SpawnLocation,
                ArrowRotation + FRotator(0.f, -90.f, 0.f)
            );
        }

        // Spawn projectile
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Character;
        SpawnParams.Instigator = Character;

        GetWorld()->SpawnActor<AActor>(
            ArcherProjectileClass,
            SpawnLocation,
            ArrowRotation,
            SpawnParams
        );
    }
}
