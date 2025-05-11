// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BDNeutralMonsterBase.generated.h"

class UBDHealthComponent;
class UBDMonsterFSMComponent;
class UBDPawnSensingComponent;
class UBDMonsterDropComponent;

UCLASS()
class BLACKDESERT_API ABDNeutralMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ABDNeutralMonsterBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 300.f;  // 몬스터마다 에디터에서 개별 설정

protected:
	virtual void BeginPlay() override;
	

	/** 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDMonsterFSMComponent* FSMComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDPawnSensingComponent* SensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBDMonsterDropComponent* DropComp;

	void DestroyAfterDeath();

	/** Sensing 콜백 */
	UFUNCTION() void OnSeePawn(APawn* Pawn);


	/** 죽음 처리 */
	UFUNCTION() void HandleDeath();


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
