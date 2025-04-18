// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Monster/Component/BDMonsterFSMComponent.h"
#include "BDMonsterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// ���� FSM ����
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EMonsterState CurrentState;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	APawn* OwningPawn;

	UPROPERTY(BlueprintReadOnly)
	UBDMonsterFSMComponent* FSM;
};
