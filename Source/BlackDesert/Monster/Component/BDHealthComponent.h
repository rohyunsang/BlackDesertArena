// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BDHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Monster), meta=(BlueprintSpawnableComponent) )
class BLACKDESERT_API UBDHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBDHealthComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHP = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHP;

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	/** 데미지 적용 (음수면 회복도 가능) */
	void ApplyDamage(float Amount);

		
};
