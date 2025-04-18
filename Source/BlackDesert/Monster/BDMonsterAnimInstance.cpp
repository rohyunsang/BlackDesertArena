// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BDMonsterAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Monster/Component/BDMonsterFSMComponent.h"

void UBDMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningPawn)
		OwningPawn = TryGetPawnOwner();

	if (!OwningPawn) return;

	if (!FSM)
		FSM = OwningPawn->FindComponentByClass<UBDMonsterFSMComponent>();

	if (FSM)
	{
		CurrentState = FSM->GetState();
	}
}