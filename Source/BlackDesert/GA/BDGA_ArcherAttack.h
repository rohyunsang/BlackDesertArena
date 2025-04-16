// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/BDGA_BasicAttack.h"
#include "BDGA_ArcherAttack.generated.h"

// 나이아가라 사용을 위한 선언
class UNiagaraSystem;


/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDGA_ArcherAttack : public UBDGA_BasicAttack
{
	GENERATED_BODY()


public:
    UBDGA_ArcherAttack();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;


protected:
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TSubclassOf<AActor> ArcherProjectileClass;


protected:
    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    UNiagaraSystem* ShootEffectVFX;

};
