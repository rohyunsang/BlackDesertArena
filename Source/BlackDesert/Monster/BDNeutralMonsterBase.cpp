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

	// ĳ���� ȸ���� ��Ʈ�ѷ��� ���� �����ϵ��� ����
	bUseControllerRotationYaw = true;

	// MovementComponent ����
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
	// �÷��̾� �߰� �� Chase ����
	FSMComp->SetState(EMonsterState::Chase);
}


void ABDNeutralMonsterBase::HandleDeath()
{
	FSMComp->SetState(EMonsterState::Dead);
	// ���� �ִϸ��̼�/����/Destroy  ������ �� �߰�

	// ����ġ ���� ó�� 
	RewardXPToPlayer();

	// ������ ��� ó��
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

// ����ġ ���� ó�� �Լ� ���� - ���� �÷��̾� ĳ���� ���� ���
void ABDNeutralMonsterBase::RewardXPToPlayer()
{
	if (RewardXP <= 0.0f)
	{
		return;
	}

	// �÷��̾� ĳ���� ����
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

	// �÷��̾��� AttributeSet ����
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

	// ����ġ ���� ����
	AttrSet->AddXP(RewardXP);

	UE_LOG(LogTemp, Log, TEXT("Successfully added %.1f XP to player (manual AddXP)"), RewardXP);
}
