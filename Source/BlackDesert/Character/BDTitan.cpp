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


	// /Script/Engine.Blueprint'/Game/Blueprint/GA/Titan/BPGA_TitanPrimary.BPGA_TitanPrimary'
	static ConstructorHelpers::FClassFinder<UGameplayAbility> TitanSkill1BP(
		TEXT("/Game/Blueprint/GA/Titan/BPGA_TitanPrimary")
	);

	// /Script/Engine.Blueprint'/Game/Blueprint/GA/Titan/BPGA_TitanSecondary.BPGA_TitanSecondary'

	static ConstructorHelpers::FClassFinder<UGameplayAbility> TitanSkill2BP(
		TEXT("/Game/Blueprint/GA/Titan/BPGA_TitanSecondary")
	);

	// /Script/Engine.Blueprint'/Game/Blueprint/GA/Titan/BPGA_TitanTertiary.BPGA_TitanTertiary'
	// /Script/Engine.Blueprint'/Game/Blueprint/GA/Titan/BPGA_TitanUltimate.BPGA_TitanUltimate'

	static ConstructorHelpers::FClassFinder<UGameplayAbility> TitanSkill3BP(
		TEXT("/Game/Blueprint/GA/Titan/BPGA_TitanTertiary")
	);

	static ConstructorHelpers::FClassFinder<UGameplayAbility> TitanSkill4BP(
		TEXT("/Game/Blueprint/GA/Titan/BPGA_TitanUltimate")
	);

	if (TitanAttackBP.Succeeded())
	{
		TitanAttackAbilityBPClass = TitanAttackBP.Class;
	}
	if (TitanSkill1BP.Succeeded())
	{
		TitanSkill1AbilityBPClass = TitanSkill1BP.Class;
	}
	if (TitanSkill2BP.Succeeded())
	{
		TitanSkill2AbilityBPClass = TitanSkill2BP.Class;
	}
	if (TitanSkill3BP.Succeeded())
	{
		TitanSkill3AbilityBPClass = TitanSkill3BP.Class;
	}
	if (TitanSkill4BP.Succeeded())
	{
		TitanSkill4AbilityBPClass = TitanSkill4BP.Class;
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
	if (AbilitySystemComponent && TitanSkill1AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Primary Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(TitanSkill1AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && TitanSkill2AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Secondary Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(TitanSkill2AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && TitanSkill3AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Tertiary Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(TitanSkill3AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && TitanSkill4AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Ultimate Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(TitanSkill4AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
}
