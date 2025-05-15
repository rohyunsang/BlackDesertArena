// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BDIgneous.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ConstructorHelpers.h"

ABDIgneous::ABDIgneous()
{

	static ConstructorHelpers::FClassFinder<UGameplayAbility> IgneousAttackBP(
		TEXT("/Game/Blueprint/GA/Igneous/BPGA_IgneousAttack")
	);
	static ConstructorHelpers::FClassFinder<UGameplayAbility> IgneousSkill1BP(
		TEXT("/Game/Blueprint/GA/Igneous/BPGA_IgneousPrimary")
	);
	static ConstructorHelpers::FClassFinder<UGameplayAbility> IgneousSkill2BP(
		TEXT("/Game/Blueprint/GA/Igneous/BPGA_IgneousSecondary")
	);
	static ConstructorHelpers::FClassFinder<UGameplayAbility> IgneousSkill3BP(
		TEXT("/Game/Blueprint/GA/Igneous/BPGA_IgneousTertiary")
	);
	static ConstructorHelpers::FClassFinder<UGameplayAbility> IgneousSkill4BP(
		TEXT("/Game/Blueprint/GA/Igneous/BPGA_IgneousUltimate")
	);

	if (IgneousAttackBP.Succeeded())
	{
		IgneousAttackAbilityBPClass = IgneousAttackBP.Class;
	}
	if (IgneousSkill1BP.Succeeded())
	{
		IgneousSkill1AbilityBPClass = IgneousSkill1BP.Class;
	}
	if (IgneousSkill2BP.Succeeded())
	{
		IgneousSkill2AbilityBPClass = IgneousSkill2BP.Class;
	}
	if (IgneousSkill3BP.Succeeded())
	{
		IgneousSkill3AbilityBPClass = IgneousSkill3BP.Class;
	}
	if (IgneousSkill4BP.Succeeded())
	{
		IgneousSkill4AbilityBPClass = IgneousSkill4BP.Class;
	}
}

void ABDIgneous::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BD_LOG Igneous BeginPlay called"));
}

void ABDIgneous::InitializeAbilities()
{
	if (AbilitySystemComponent && IgneousAttackAbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG InitializeAbilities"));
		FGameplayAbilitySpec AttackSpec(IgneousAttackAbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(AttackSpec);
	}
	if (AbilitySystemComponent && IgneousSkill1AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Primary Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(IgneousSkill1AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && IgneousSkill2AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Secondary Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(IgneousSkill2AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && IgneousSkill3AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Tertiary Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(IgneousSkill3AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && IgneousSkill4AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Ultimate Titan Attack InitializeAbilities"));
		FGameplayAbilitySpec PrimaryAttackSpec(IgneousSkill4AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
}
