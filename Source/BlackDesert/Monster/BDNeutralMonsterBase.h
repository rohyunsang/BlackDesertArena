// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BDNeutralMonsterBase.generated.h"

class UBDHealthComponent;
class UBDMonsterFSMComponent;
class UBDPawnSensingComponent;
class UBDMonsterDropComponent;
class UGameplayEffect;

UCLASS()
class BLACKDESERT_API ABDNeutralMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ABDNeutralMonsterBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 300.f;  // ���͸��� �����Ϳ��� ���� ����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> ExpGE;

	// ���Ͱ� ������ ����ġ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
	float RewardXP;


private:
	// ����ġ ���� ó�� �Լ�
	void RewardXPToPlayer();

protected:
	virtual void BeginPlay() override;
	

	/** ������Ʈ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDMonsterFSMComponent* FSMComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UBDPawnSensingComponent* SensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBDMonsterDropComponent* DropComp;

	void DestroyAfterDeath();

	/** Sensing �ݹ� */
	UFUNCTION() void OnSeePawn(APawn* Pawn);


	/** ���� ó�� */
	UFUNCTION() void HandleDeath();


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
