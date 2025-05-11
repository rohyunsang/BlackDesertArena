// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BDNeutralMonsterAIController.generated.h"

class UBDMonsterFSMComponent;

/**
 * 
 */
UCLASS()
class BLACKDESERT_API ABDNeutralMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABDNeutralMonsterAIController();

	/** �߰� ���� (FSM ���� ȣ��) */
	void StartChase(APawn* InTarget);

	/** Patrol */
	void StartPatrol();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;

	/** MoveTo ���� �ݹ� */
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	/** ���� ��Ÿ�(�Ÿ� �� AttackRange �� Attack ����) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRange = 300.f;   // ���� ��Ÿ� 

private:
	UPROPERTY()
	APawn* ControlledPawn = nullptr;

	UPROPERTY()
	UBDMonsterFSMComponent* FSM = nullptr;

	UPROPERTY()
	APawn* TargetPawn = nullptr;

	UPROPERTY(EditAnywhere, Category = "AI")
	float PatrolRadius = 500.f;

private:
	FVector GetRandomPatrolLocation() const;


};

