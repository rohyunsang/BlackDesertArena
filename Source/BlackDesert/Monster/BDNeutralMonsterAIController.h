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

	/** 추격 시작 (FSM 에서 호출) */
	void StartChase(APawn* InTarget);

	/** Patrol */
	void StartPatrol();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;

	/** MoveTo 종료 콜백 */
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	/** 공격 사거리(거리 ≤ AttackRange → Attack 상태) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRange = 300.f;   // 공격 사거리 

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

