// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BDTitan.h"
#include "GA/BDGA_TitanAttack.h" 
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ConstructorHelpers.h"


ABDTitan::ABDTitan()
{
	static ConstructorHelpers::FClassFinder<UGameplayAbility> TitanAttackBP(
		TEXT("/Game/Blueprint/GA/Titan/BPGA_TitanAttack")
	);
	// /Script/Engine.Blueprint'/Game/Blueprint/GA/Titan/BPGA_TitanAttack.BPGA_TitanAttack'

	// /Script/Engine.Blueprint'/Game/Blueprint/GA/GA_BP_ArcherPrimary.GA_BP_ArcherPrimary'

	static ConstructorHelpers::FClassFinder<UGameplayAbility> TitanPrimaryAttackBP(
		TEXT("/Game/Blueprint/GA/GA_BP_ArcherPrimary")
	);

	if (TitanAttackBP.Succeeded())
	{
		TitanAttackAbilityBPClass = TitanAttackBP.Class;
	}
	if (TitanPrimaryAttackBP.Succeeded())
	{
		TitanPrimaryAttackAbilityBPClass = TitanPrimaryAttackBP.Class;
	}
}

void ABDTitan::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BD_LOG Titan BeginPlay called"));
}

void ABDTitan::InitializeAbilities()
{
	if (AbilitySystemComponent && TitanAttackAbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG InitializeAbilities"));
		FGameplayAbilitySpec AttackSpec(TitanAttackAbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(AttackSpec);
	}
	if (AbilitySystemComponent && TitanPrimaryAttackAbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Primary Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(TitanPrimaryAttackAbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
}
