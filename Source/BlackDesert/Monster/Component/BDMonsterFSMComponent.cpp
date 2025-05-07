// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Component/BDMonsterFSMComponent.h"
#include "GameFramework/Actor.h"
#include "Monster/BDNeutralMonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h" 

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

	case EMonsterState::Idle:   HandleIdle(DeltaTime);   break;
	case EMonsterState::Patrol: HandlePatrol(DeltaTime); break;
	case EMonsterState::Chase:  HandleChase(DeltaTime);  break;
	case EMonsterState::Attack: HandleAttack(DeltaTime); break;
	case EMonsterState::Dead:   HandleDead(DeltaTime);   break;

	default: break; 
	}
}

void UBDMonsterFSMComponent::SetState(EMonsterState NewState)
{
	if (State == NewState) return;

	// 이미 Dead 상태라면 상태 변경을 허용하지 않음
	if (State == EMonsterState::Dead)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FSM] Ignoring state change to %s: Monster is already dead"),
			*UEnum::GetValueAsString(NewState));
		return;
	}

	// 이전 상태 캐시
	EMonsterState PrevState = State;
	State = NewState;

	UE_LOG(LogTemp, Warning, TEXT("[FSM] Changing state from %s to %s"),
		*UEnum::GetValueAsString(PrevState),
		*UEnum::GetValueAsString(NewState));

	// 이동 중이라면 AI 이동 멈춤
	if (ABDNeutralMonsterAIController* AI = Cast<ABDNeutralMonsterAIController>(Cast<APawn>(GetOwner())->GetController()))
	{
		if (NewState == EMonsterState::Idle || NewState == EMonsterState::Dead)
		{
			AI->StopMovement(); // ←  이거 중요함!
		}

		switch (NewState)
		{
		case EMonsterState::Patrol: AI->StartPatrol(); break;
		case EMonsterState::Chase:
		{
			APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			if (Player) AI->StartChase(Player);
			break;
		}
		default: break;
		}
	}
}

void UBDMonsterFSMComponent::HandleIdle(float)
{
	// 임시 로그
	// UE_LOG(LogTemp, Log, TEXT("[%s] Monster State :  Idle..."), *GetOwner()->GetName());
}

void UBDMonsterFSMComponent::HandlePatrol(float Delta)
{
	UE_LOG(LogTemp, Log, TEXT("[%s] Monster State : Patrol"), *GetOwner()->GetName());
	// AIController가 MoveTo 호출해서 순찰 이동

	if (AController* Ctrl = Cast<AController>(GetOwner()->GetInstigatorController()))
	{
		if (ABDNeutralMonsterAIController* AI = Cast<ABDNeutralMonsterAIController>(Ctrl))
		{
			AI->StartPatrol();
		}
	}
}

void UBDMonsterFSMComponent::HandleChase(float Delta)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	float Distance = FVector::Dist(OwnerPawn->GetActorLocation(), Player->GetActorLocation());

	// 감지 범위를 벗어난 경우 → Idle 상태로 전환
	const float LoseSightRadius = 900.f;
	if (Distance > LoseSightRadius)
	{
		SetState(EMonsterState::Idle);
		return;
	}


	// 계속 추격
	if (ABDNeutralMonsterAIController* AI = Cast<ABDNeutralMonsterAIController>(OwnerPawn->GetController()))
	{
		AI->StartChase(Player);
	}
}


void UBDMonsterFSMComponent::HandleAttack(float Delta)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return; // 아직 쿨타임 안 지남
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] Monster State : Attack!"), *GetOwner()->GetName());

	LastAttackTime = CurrentTime;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		const float LaunchPower = 2000.f;

		// 현재 회전값 가져와서 Yaw +90도 추가
		FRotator RotatedYaw = OwnerCharacter->GetActorRotation();
		RotatedYaw.Yaw += 90.f;

		// 회전된 방향에서 Forward(X축) 벡터 구함
		FVector RotatedForward = FRotationMatrix(RotatedYaw).GetUnitAxis(EAxis::X);

		OwnerCharacter->LaunchCharacter(RotatedForward * LaunchPower, true, false);
	}

	PlayAttackMontage();
}

void UBDMonsterFSMComponent::HandleDead(float)
{
	// 이미 처리된 경우 바로 반환
	if (bDeadHandled)
		return;

	bDeadHandled = true;

	UE_LOG(LogTemp, Log, TEXT("[%s] Monster State : Dead"), *GetOwner()->GetName());

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !DeadMontage)
	{
		return;
	}
	AnimInstance->Montage_Play(DeadMontage);
}


void UBDMonsterFSMComponent::PlayAttackMontage()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) {
		return;
	} 

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !AttackMontage) {
		return;
	} 
	AnimInstance->Montage_Play(AttackMontage);
}


void UBDMonsterFSMComponent::PerformAttackHitCheck()
{
	UE_LOG(LogTemp, Log, TEXT("PerformAttackHitCheck Called"));

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerCharacter is null"));
		return;
	}

	// 회전 보정된 방향 계산
	FRotator RotatedYaw = OwnerCharacter->GetActorRotation();
	RotatedYaw.Yaw += 90.f; // 고정된 방향 보정

	FVector ForwardDirection = FRotationMatrix(RotatedYaw).GetUnitAxis(EAxis::X);
	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start + ForwardDirection * AttackRange;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.5f, 0, 2.0f);

	UE_LOG(LogTemp, Log, TEXT("Trace Start: %s"), *Start.ToString());
	UE_LOG(LogTemp, Log, TEXT("Trace End: %s"), *End.ToString());

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);

	if (bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *GetNameSafe(Hit.GetActor()));

		if (Hit.GetActor())
		{
			UE_LOG(LogTemp, Log, TEXT("Applying Damage: %f"), AttackDamage);
			UGameplayStatics::ApplyDamage(Hit.GetActor(), AttackDamage, nullptr, OwnerCharacter, nullptr);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No actor hit"));
	}
}

