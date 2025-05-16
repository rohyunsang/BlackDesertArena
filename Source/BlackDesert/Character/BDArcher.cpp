// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/BDArcher.h"
#include "GA/BDGA_ArcherAttack.h" 
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ConstructorHelpers.h"

ABDArcher::ABDArcher()
{
	static ConstructorHelpers::FClassFinder<UGameplayAbility> ArcherAttackBP(
		TEXT("/Game/Blueprint/GA/GA_BP_ArcherAttack")
	);
	static ConstructorHelpers::FClassFinder<UGameplayAbility> ArcherSkill1BP(
		TEXT("/Game/Blueprint/GA/Fletcher/BDGA_FletcherPrimary")
	);
	static ConstructorHelpers::FClassFinder<UGameplayAbility> ArcherSkill2BP(
		TEXT("/Game/Blueprint/GA/Fletcher/BDGA_FletcherSecondary")
	); 
	static ConstructorHelpers::FClassFinder<UGameplayAbility> ArcherSkill3BP(
		TEXT("/Game/Blueprint/GA/Fletcher/BDGA_FletcherTertiary")
	);
	static ConstructorHelpers::FClassFinder<UGameplayAbility> ArcherSkill4BP(
		TEXT("/Game/Blueprint/GA/Fletcher/BDGA_FletcherUltimate")
	);


	if (ArcherAttackBP.Succeeded())
	{
		ArcherAttackAbilityBPClass = ArcherAttackBP.Class;
	}
	if (ArcherSkill1BP.Succeeded())
	{
		ArcherSkill1AbilityBPClass = ArcherSkill1BP.Class;
	}
	if (ArcherSkill2BP.Succeeded())
	{
		ArcherSkill2AbilityBPClass = ArcherSkill2BP.Class;
	}
	if (ArcherSkill3BP.Succeeded())
	{
		ArcherSkill3AbilityBPClass = ArcherSkill3BP.Class;
	}
	if (ArcherSkill4BP.Succeeded())
	{
		ArcherSkill4AbilityBPClass = ArcherSkill4BP.Class;
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
	if (AbilitySystemComponent && ArcherSkill1AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Primary Archer Attack InitializeAbilities"));
		// "Input.Action.Primary" 태그는 GA_BP_ArcherAttack 안에 있어야 함
		FGameplayAbilitySpec PrimaryAttackSpec(ArcherSkill1AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && ArcherSkill2AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Secondary Archer Attack InitializeAbilities"));
		// "Input.Action.Secondary" 태그는 GA_BP_ArcherAttack 안에 있어야 함
		FGameplayAbilitySpec PrimaryAttackSpec(ArcherSkill2AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && ArcherSkill3AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Tertiary Archer Attack InitializeAbilities"));
		// "Input.Action.Tertiary" 태그는 GA_BP_ArcherAttack 안에 있어야 함
		FGameplayAbilitySpec PrimaryAttackSpec(ArcherSkill3AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
	if (AbilitySystemComponent && ArcherSkill4AbilityBPClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG Ultimate Archer Attack InitializeAbilities"));
		// "Input.Action.Ultimate" 태그는 GA_BP_ArcherAttack 안에 있어야 함
		FGameplayAbilitySpec PrimaryAttackSpec(ArcherSkill4AbilityBPClass, 1);
		AbilitySystemComponent->GiveAbility(PrimaryAttackSpec);
	}
}