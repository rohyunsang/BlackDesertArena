// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/BDArcher.h"
#include "GA/BDGA_ArcherAttack.h" 
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ConstructorHelpers.h"

ABDArcher::ABDArcher()
{
	// /Script/Engine.Blueprint'/Game/Blueprint/GA/GA_BP_ArcherAttack.GA_BP_ArcherAttack'
	// /Script/Engine.Blueprint'/Game/Blueprint/GA/GA_BP_ArcherAttack.GA_BP_ArcherAttack'
	static ConstructorHelpers::FClassFinder<UGameplayAbility> ArcherAttackBP(
		TEXT("/Game/Blueprint/GA/GA_BP_ArcherAttack")
	);

	if (ArcherAttackBP.Succeeded())
	{
		ArcherAttackAbilityBPClass = ArcherAttackBP.Class;
	}
}

void ABDArcher::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BD_LOG Archer BeginPlay called"));
}

void ABDArcher::InitializeAbilities()
{
	// UE_LOG(LogTemp, Warning, TEXT("BD_LOG InitializeAbilities: %s"), *ArcherAttackAbilityBPClass->GetName());
	if (AbilitySystemComponent && ArcherAttackAbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG InitializeAbilities"));
		// "Input.Action.Attack" 태그는 GA_BP_ArcherAttack 안에 있어야 함
		FGameplayAbilitySpec Spec(ArcherAttackAbilityBPClass, 1, 0);
		AbilitySystemComponent->GiveAbility(Spec);
	}
}