// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BDGA_SkillBase.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDGA_SkillBase : public UGameplayAbility
{
	GENERATED_BODY()
public:
    UBDGA_SkillBase();

protected:
    // ��ų ��Ÿ�� ����
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
    FGameplayTag CooldownTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
    float CooldownDuration = 1.0f;

    // ����Ʈ or ��ų ȿ�� �ߵ���
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
    TSubclassOf<UGameplayEffect> CooldownEffect;


protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    UAnimMontage* AttackMontage;


public:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;
};
