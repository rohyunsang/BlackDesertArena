// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlackDesertCharacter.h"
#include "BDIgneous.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API ABDIgneous : public ABlackDesertCharacter
{
	GENERATED_BODY()
	
public:
	ABDIgneous();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilities() override; // override for parentClass ; 


private:
	// GA_BP_ArcherAttack Blueprint class reference
	TSubclassOf<class UGameplayAbility> IgneousAttackAbilityBPClass;

	TSubclassOf<class UGameplayAbility> IgneousSkill1AbilityBPClass;

	TSubclassOf<class UGameplayAbility> IgneousSkill2AbilityBPClass;

	TSubclassOf<class UGameplayAbility> IgneousSkill3AbilityBPClass;

	TSubclassOf<class UGameplayAbility> IgneousSkill4AbilityBPClass;

};
