// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BDNeutralMonsterAIController.h"
#include "Component/BDMonsterFSMComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABDNeutralMonsterAIController::ABDNeutralMonsterAIController()
{
	// AIController Tick �ʿ� ������ ���� ��
	PrimaryActorTick.bCanEverTick = true;
}

void ABDNeutralMonsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetPawn && ControlledPawn)
	{
		FVector Direction = (TargetPawn->GetActorLocation() - ControlledPawn->GetActorLocation());
		Direction.Z = 0.0f; // ���� ȸ���� ���

		FRotator LookAtRot = Direction.Rotation();
		LookAtRot.Pitch = 0.0f;
		LookAtRot.Roll = 0.0f;

		// �������� �ִٸ� ���⼭ Yaw ���� (��: ���� ������ -Y���̶�� -90��)
		LookAtRot.Yaw -= 90.0f;

		ControlledPawn->SetActorRotation(LookAtRot);
	}
}

void ABDNeutralMonsterAIController::StartPatrol()
{
	if (!ControlledPawn) return;

	FVector PatrolDest = GetRandomPatrolLocation();

	MoveToLocation(PatrolDest, /*AcceptanceRadius*/ 10.f);
}

FVector ABDNeutralMonsterAIController::GetRandomPatrolLocation() const
{
	if (!ControlledPawn) return FVector::ZeroVector;

	const FVector Origin = ControlledPawn->GetActorLocation();

	// ���� ���� ��ġ
	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	const float Radius = FMath::FRandRange(100.f, PatrolRadius);

	const FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Radius;

	return Origin + Offset;
}

void ABDNeutralMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledPawn = InPawn;
	if (ControlledPawn)
	{
		// FSM ������Ʈ ĳ��
		FSM = ControlledPawn->FindComponentByClass<UBDMonsterFSMComponent>();
	}
}

void ABDNeutralMonsterAIController::StartChase(APawn* InTarget)
{
	if (!InTarget || !ControlledPawn) return;

	// ȸ�� ��ǥ�� �����Ͽ� ���Ͱ� Ÿ���� �ٶ󺸰� ��
	// SetFocalPoint(InTarget->GetActorLocation());

	if (GetMoveStatus() == EPathFollowingStatus::Moving) return;


	TargetPawn = InTarget;

	UE_LOG(LogTemp, Warning, TEXT("[AI] StartChase Called. Target: %s"), *InTarget->GetName());

	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), TargetPawn->GetActorLocation());
	UE_LOG(LogTemp, Warning, TEXT("[AI] Distance to Target: %.1f"), Distance);

	EPathFollowingRequestResult::Type Result = MoveToActor(TargetPawn, AttackRange - 10.f, true, true);
	UE_LOG(LogTemp, Warning, TEXT("[AI] MoveToActor Result: %d"), (int32)Result);
}

void ABDNeutralMonsterAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (!FSM || !ControlledPawn) return;

	switch (FSM->GetState())
	{
	case EMonsterState::Chase:
	{
		float Dist = FVector::Dist(ControlledPawn->GetActorLocation(), TargetPawn->GetActorLocation());
		if (Dist <= AttackRange)
		{
			// ���� ���� ���̸� ���� ���� ���� ���� ���� ���� ����
			if (FSM->GetState() != EMonsterState::Attack)
			{
				FSM->SetState(EMonsterState::Attack);
				UE_LOG(LogTemp, Warning, TEXT("[AI] Attack"));
			}
		}
		else
		{
			FSM->SetState(EMonsterState::Chase); // ���� �ٱ��̸� �߰� ���
		}
		break;
	}
	case EMonsterState::Patrol:
		FSM->SetState(EMonsterState::Patrol);
		break;
	default: break;
	}
}
