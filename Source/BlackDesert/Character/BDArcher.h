// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlackDesertCharacter.h"
#include "BDArcher.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API ABDArcher : public ABlackDesertCharacter
{
	GENERATED_BODY()
	

public:
	ABDArcher();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilities() override; // override for parentClass ; 


private:
	// GA_BP_ArcherAttack Blueprint class reference
	TSubclassOf<class UGameplayAbility> ArcherAttackAbilityBPClass;
};
