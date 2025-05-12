// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlackDesertCharacter.h"
#include "BDTitan.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API ABDTitan : public ABlackDesertCharacter
{
	GENERATED_BODY()
	
public:
	ABDTitan();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilities() override; // override for parentClass ; 


private:
	// GA_BP_ArcherAttack Blueprint class reference
	TSubclassOf<class UGameplayAbility> TitanAttackAbilityBPClass;

	TSubclassOf<class UGameplayAbility> TitanPrimaryAttackAbilityBPClass;
};
