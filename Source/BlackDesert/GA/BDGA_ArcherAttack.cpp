// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_ArcherAttack.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"
#include "Actor/BDArcherProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"


UBDGA_ArcherAttack::UBDGA_ArcherAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    // Blueprint로 만든 Projectile 클래스 참조용
    static ConstructorHelpers::FClassFinder<AActor> ProjectileBPClass(TEXT("/Game/Blueprint/Weapon/BP_ArcherProjectile.BP_ArcherProjectile_C"));

    if (ProjectileBPClass.Succeeded())
    {
        ArcherProjectileClass = ProjectileBPClass.Class;
    }
}

void UBDGA_ArcherAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    UE_LOG(LogTemp, Warning, TEXT("BD_LOG GA_ArcherAttack::ActivateAbility called!"));

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (Character && ArcherProjectileClass)
    {
        // 화살 발사 위치 (캐릭터 위치 기준 + 오프셋)
        FVector MuzzleLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
        FRotator MuzzleRotation = Character->GetActorRotation(); // 캐릭터 바라보는 방향

        //  궤적 이펙트 실행
        if (ShootEffectVFX)
        {
            FVector Direction = Character->GetActorForwardVector(); // 캐릭터 바라보는 방향
            MuzzleLocation = Character->GetActorLocation() + Direction * 100.0f + FVector(0, 0, 50.0f);
            MuzzleRotation = Direction.Rotation();  // ← 여기서 방향 회전값 재설정

            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                ShootEffectVFX,
                MuzzleLocation,
                MuzzleRotation
            );

            UE_LOG(LogTemp, Warning, TEXT("BD_LOG ShootEffectVFX Spawned at Direction %s"), *Direction.ToString());
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Character;
        SpawnParams.Instigator = Character;

        AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(
            ArcherProjectileClass,
            MuzzleLocation,
            MuzzleRotation,
            SpawnParams
        );

        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Arrow Spawned!"));
    }

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
