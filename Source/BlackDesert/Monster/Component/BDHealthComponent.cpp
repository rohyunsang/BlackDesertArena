// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Component/BDHealthComponent.h"

UBDHealthComponent::UBDHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHP = MaxHP;
}

void UBDHealthComponent::ApplyDamage(float Amount)
{
	if (bIsDead)
	{
		return;
	}

	CurrentHP = FMath::Clamp(CurrentHP - Amount, 0.f, MaxHP);

	UE_LOG(LogTemp, Warning, TEXT(" Current health: %.1f / %.1f"), CurrentHP, MaxHP);

	if (CurrentHP <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT(" death "));
		bIsDead = true;
		OnDeath.Broadcast();
	}
}

