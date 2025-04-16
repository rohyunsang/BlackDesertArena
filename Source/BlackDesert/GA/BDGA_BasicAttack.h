// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BDGA_BasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDGA_BasicAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
    UBDGA_BasicAttack();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    UAnimMontage* AttackMontage;



	
};
