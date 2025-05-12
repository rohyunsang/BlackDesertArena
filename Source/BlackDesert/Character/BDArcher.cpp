// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/BDArcher.h"
#include "GA/BDGA_ArcherAttack.h" 
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ConstructorHelpers.h"

ABDArcher::ABDArcher()
{
	// /Script/Engine.Blueprint'/Game/Blueprint/GA/GA_BP_ArcherAttack.GA_BP_ArcherAttack'

	static ConstructorHelpers::FClassFinder<UGameplayAbility> ArcherAttackBP(
		TEXT("/Game/Blueprint/GA/GA_BP_ArcherAttack")
	);


	// /Script/Engine.Blueprint'/Game/Blueprint/GA/GA_BP_ArcherPrimary.GA_BP_ArcherPrimary'

	static ConstructorHelpers::FClassFinder<UGameplayAbility> ArcherPrimaryAttackBP(
		TEXT("/Game/Blueprint/GA/GA_BP_ArcherPrimary")
	);

	if (ArcherAttackBP.Succeeded())
	{
		ArcherAttackAbilityBPClass = ArcherAttackBP.Class;
	}
	if (ArcherPrimaryAttackBP.Succeeded())
	{
		ArcherPrimaryAttackAbilityBPClass = ArcherPrimaryAttackBP.Class;
	}
}

void ABDArcher::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BD_LOG Archer BeginPlay called"));
}

void ABDArcher::InitializeAbilities()
{
	if (AbilitySystemComponent && ArcherAttackAbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG InitializeAbilities"));
		// "Input.Action.Attack" 태그는 GA_BP_ArcherAttack 안에 있어야 함
		FGameplayAbilitySpec AttackSpec(ArcherAttackAbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(AttackSpec);
	}
	if (AbilitySystemComponent && ArcherPrimaryAttackAbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Primary Archer Attack InitializeAbilities"));
		// "Input.Action.Attack" 태그는 GA_BP_ArcherAttack 안에 있어야 함
		FGameplayAbilitySpec PrimaryAttackSpec(ArcherPrimaryAttackAbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
}