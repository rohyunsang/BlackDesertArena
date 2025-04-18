// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BDMonsterFSMComponent.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Idle,     // 가만히 있음
	Patrol,   // 이동 중
	Chase,    // 플레이어 추격
	Attack,   // 공격 애님 재생 중
	Dead
};


UCLASS( ClassGroup=(Monster), meta=(BlueprintSpawnableComponent) )
class BLACKDESERT_API UBDMonsterFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBDMonsterFSMComponent();

	void SetState(EMonsterState NewState);
	EMonsterState GetState() const { return State; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	EMonsterState State = EMonsterState::Idle;

	// 간단 확인용
	void HandleIdle(float Delta);
	void HandleDead(float Delta);
		
};
