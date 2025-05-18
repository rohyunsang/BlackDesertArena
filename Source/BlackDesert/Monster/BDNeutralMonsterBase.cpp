// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BDNeutralMonsterBase.h"
#include "Component/BDHealthComponent.h"
#include "Component/BDMonsterFSMComponent.h"
#include "Component/BDMonsterDropComponent.h"
#include "BDPawnSensingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Attribute/BDAttributeSet.h"
#include "BlackDesertCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"

ABDNeutralMonsterBase::ABDNeutralMonsterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터 회전을 컨트롤러가 직접 제어하도록 설정
	bUseControllerRotationYaw = true;

	// MovementComponent 설정
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	HealthComp = CreateDefaultSubobject<UBDHealthComponent>(TEXT("HealthComp"));
	FSMComp = CreateDefaultSubobject<UBDMonsterFSMComponent>(TEXT("FSMComp"));
	SensingComp = CreateDefaultSubobject<UBDPawnSensingComponent>(TEXT("SensingComp"));
	DropComp = CreateDefaultSubobject<UBDMonsterDropComponent>(TEXT("DropComp"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

    // RewardXP = 10.0f;
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

	// 경험치 보상 처리 
	RewardXPToPlayer();

	// 아이템 드롭 처리
	if (DropComp)
	{
		DropComp->DropLoot();
	}

	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ABDNeutralMonsterBase::DestroyAfterDeath, 1.3f, false);
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

void ABDNeutralMonsterBase::DestroyAfterDeath()
{
	Destroy();
}

// 경험치 보상 처리 함수 구현 - 직접 플레이어 캐릭터 접근 방식
void ABDNeutralMonsterBase::RewardXPToPlayer()
{
	if (RewardXP <= 0.0f)
	{
		return;
	}

	// 플레이어 캐릭터 접근
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find player controller for XP reward."));
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find player pawn for XP reward."));
		return;
	}

	// 플레이어의 AttributeSet 접근
	ABlackDesertCharacter* BDPlayer = Cast<ABlackDesertCharacter>(PlayerPawn);
	if (!BDPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is not a valid ABlackDesertCharacter."));
		return;
	}

	UBDAttributeSet* AttrSet = const_cast<UBDAttributeSet*>(BDPlayer->GetAbilitySystemComponent()->GetSet<UBDAttributeSet>());
	if (!AttrSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get UBDAttributeSet from player."));
		return;
	}

	// 경험치 수동 적용
	AttrSet->AddXP(RewardXP);

	UE_LOG(LogTemp, Log, TEXT("Successfully added %.1f XP to player (manual AddXP)"), RewardXP);
}
