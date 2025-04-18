// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BDNeutralMonsterBase.h"
#include "Component/BDHealthComponent.h"
#include "Component/BDMonsterFSMComponent.h"
#include "BDPawnSensingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"


ABDNeutralMonsterBase::ABDNeutralMonsterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComp = CreateDefaultSubobject<UBDHealthComponent>(TEXT("HealthComp"));
	FSMComp = CreateDefaultSubobject<UBDMonsterFSMComponent>(TEXT("FSMComp"));
	SensingComp = CreateDefaultSubobject<UBDPawnSensingComponent>(TEXT("SensingComp"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
}

void ABDNeutralMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	SensingComp->OnSeePawn.AddDynamic(this, &ABDNeutralMonsterBase::OnSeePawn);
	HealthComp->OnDeath.AddDynamic(this, &ABDNeutralMonsterBase::HandleDeath);
}

void ABDNeutralMonsterBase::OnSeePawn(APawn* Pawn)
{
	// 플레이어 발견 → Chase 상태
	FSMComp->SetState(EMonsterState::Chase);
}


void ABDNeutralMonsterBase::HandleDeath()
{
	FSMComp->SetState(EMonsterState::Dead);
	// 죽음 애니메이션/루팅/Destroy  딜레이 등 추가
	Destroy();
}

float ABDNeutralMonsterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT(" TakeDamage: %.1f"), DamageAmount);

	if (HealthComp)
	{
		HealthComp->ApplyDamage(DamageAmount);
	}
	return DamageAmount;
}