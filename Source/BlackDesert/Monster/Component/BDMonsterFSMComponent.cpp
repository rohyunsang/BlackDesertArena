// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Component/BDMonsterFSMComponent.h"
#include "GameFramework/Actor.h"


UBDMonsterFSMComponent::UBDMonsterFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBDMonsterFSMComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBDMonsterFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (State)
	{
	case EMonsterState::Idle:  HandleIdle(DeltaTime);  break;
	case EMonsterState::Dead:  HandleDead(DeltaTime);  break;
	default: break; // Patrol·Chase·Attack는 이후 추가
	}
}

void UBDMonsterFSMComponent::SetState(EMonsterState NewState)
{
	State = NewState;
}

void UBDMonsterFSMComponent::HandleIdle(float)
{
	// 임시 로그
	// UE_LOG(LogTemp, Verbose, TEXT("[%s] Idle..."), *GetOwner()->GetName());
}

void UBDMonsterFSMComponent::HandleDead(float)
{
	// nothing yet
}