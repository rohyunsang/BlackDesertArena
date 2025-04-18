// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BDNeutralMonsterBase.generated.h"

class UBDHealthComponent;
class UBDMonsterFSMComponent;
class UBDPawnSensingComponent;

UCLASS()
class BLACKDESERT_API ABDNeutralMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ABDNeutralMonsterBase();

protected:
	virtual void BeginPlay() override;

	/** ÄÄÆ÷³ÍÆ® */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDMonsterFSMComponent* FSMComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDPawnSensingComponent* SensingComp;

	/** Sensing ÄÝ¹é */
	UFUNCTION() void OnSeePawn(APawn* Pawn);


	/** Á×À½ Ã³¸® */
	UFUNCTION() void HandleDeath();


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
