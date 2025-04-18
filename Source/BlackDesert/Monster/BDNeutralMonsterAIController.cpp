// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BDNeutralMonsterAIController.h"
#include "Component/BDMonsterFSMComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABDNeutralMonsterAIController::ABDNeutralMonsterAIController()
{
	// AIController Tick ÇÊ¿ä ¾øÀ¸¸é ²¨µµ µÊ
	PrimaryActorTick.bCanEverTick = false;
}

void ABDNeutralMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledPawn = InPawn;
	if (ControlledPawn)
	{
		// FSM ÄÄÆ÷³ÍÆ® Ä³½Ì
		FSM = ControlledPawn->FindComponentByClass<UBDMonsterFSMComponent>();
	}
}

void ABDNeutralMonsterAIController::StartChase(APawn* InTarget)
{
	if (!InTarget || !ControlledPawn) return;

	TargetPawn = InTarget;

	MoveToActor(TargetPawn, /*AcceptanceRadius*/AttackRange - 10.f,
		/*bStopOnOverlap*/true, /*bUsePathfinding*/true);
}

void ABDNeutralMonsterAIController::OnMoveCompleted(FAIRequestID RequestID,
	const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (!FSM || !TargetPawn) return;

	const float Dist = FVector::Dist(ControlledPawn->GetActorLocation(),
		TargetPawn->GetActorLocation());

	if (Dist <= AttackRange)
	{
		FSM->SetState(EMonsterState::Attack);
	}
	else
	{
		FSM->SetState(EMonsterState::Idle);
	}
}