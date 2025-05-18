// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Attribute/BDAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UBDAttributeSet::UBDAttributeSet()
{
	// 레벨별 필요 경험치 테이블 초기화
	XPRequirementTable = {
		0.0f,    // 레벨 0->1 (사용하지 않음)
		250.0f,  // 레벨 1->2
		550.0f,  // 레벨 2->3
		950.0f,  // 레벨 3->4
		1450.0f, // 레벨 4->5
		2050.0f, // 레벨 5->6
		0.0f     // 레벨 6은 최대 레벨 (레벨업 없음)
	};

	// 레벨별 최대 체력 테이블 초기화
	MaxHealthTable = {
		0.0f,    // 레벨 0 (사용하지 않음)
		1000.0f, // 레벨 1
		1200.0f, // 레벨 2
		1400.0f, // 레벨 3
		1600.0f, // 레벨 4
		1800.0f, // 레벨 5
		2000.0f  // 레벨 6
	};

	// 초기 레벨 및 속성 설정 (기본값, 게임플레이 이펙트로 덮어쓸 수 있음)
	InitializeLevel(1);
}

void UBDAttributeSet::InitializeLevel(int32 StartLevel)
{
	// 유효한 레벨 범위 (1~6) 확인
	StartLevel = FMath::Clamp(StartLevel, 1, 6);

	// 레벨 설정
	Level.SetCurrentValue(StartLevel);
	Level.SetBaseValue(StartLevel);

	// 경험치 초기화
	XP.SetCurrentValue(0.0f);
	XP.SetBaseValue(0.0f);

	// 다음 레벨 필요 경험치 설정
	if (StartLevel < 6)
	{
		NextLevelXP.SetCurrentValue(XPRequirementTable[StartLevel]);
		NextLevelXP.SetBaseValue(XPRequirementTable[StartLevel]);
	}
	else
	{
		// 최대 레벨인 경우
		NextLevelXP.SetCurrentValue(0.0f);
		NextLevelXP.SetBaseValue(0.0f);
	}

	// 최대 체력 설정
	MaxHealth.SetCurrentValue(MaxHealthTable[StartLevel]);
	MaxHealth.SetBaseValue(MaxHealthTable[StartLevel]);

	// 현재 체력 설정 (최대 체력과 동일하게 시작)
	Health.SetCurrentValue(MaxHealth.GetCurrentValue());
	Health.SetBaseValue(MaxHealth.GetCurrentValue());

	// 이벤트 발생
	OnLevelChanged.Broadcast(StartLevel, XP.GetCurrentValue(), NextLevelXP.GetCurrentValue());
	OnHealthChanged.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());
}

void UBDAttributeSet::AddXP(float XPAmount)
{
	// 이미 최대 레벨이면 경험치 획득 불가
	int32 CurrentLevel = FMath::FloorToInt(Level.GetCurrentValue());
	if (CurrentLevel >= 6)
	{
		return;
	}

	// 경험치 추가
	float CurrentXP = XP.GetCurrentValue();
	float NewXP = CurrentXP + XPAmount;
	XP.SetCurrentValue(NewXP);
	XP.SetBaseValue(NewXP);

	// 레벨업 조건 확인
	float RequiredXP = NextLevelXP.GetCurrentValue();
	if (NewXP >= RequiredXP)
	{
		HandleLevelUp();
	}

	UE_LOG(LogTemp, Log, TEXT("Added %f XP. Current XP: %f/%f"), XPAmount, NewXP, RequiredXP);
}

void UBDAttributeSet::HandleLevelUp()
{
	int32 CurrentLevel = FMath::FloorToInt(Level.GetCurrentValue());

	// 이미 최대 레벨이면 레벨업 불가
	if (CurrentLevel >= 6)
	{
		return;
	}

	// 현재 경험치와 필요 경험치
	float CurrentXP = XP.GetCurrentValue();
	float RequiredXP = NextLevelXP.GetCurrentValue();

	// 레벨업 가능한지 확인
	while (CurrentXP >= RequiredXP && CurrentLevel < 6)
	{
		// 레벨 증가
		CurrentLevel++;

		// 남은 경험치 계산
		CurrentXP -= RequiredXP;

		// 다음 레벨 필요 경험치 설정
		if (CurrentLevel < 6)
		{
			RequiredXP = XPRequirementTable[CurrentLevel];
		}
		else
		{
			// 최대 레벨에 도달
			RequiredXP = 0.0f;
			CurrentXP = 0.0f;
		}
	}

	// 레벨 설정
	Level.SetCurrentValue(CurrentLevel);
	Level.SetBaseValue(CurrentLevel);

	// 경험치 업데이트
	XP.SetCurrentValue(CurrentXP);
	XP.SetBaseValue(CurrentXP);

	// 다음 레벨 필요 경험치 업데이트
	NextLevelXP.SetCurrentValue(RequiredXP);
	NextLevelXP.SetBaseValue(RequiredXP);

	// 최대 체력 업데이트
	UpdateMaxHealth();

	// 이벤트 발생
	OnLevelChanged.Broadcast(CurrentLevel, CurrentXP, RequiredXP);

	UE_LOG(LogTemp, Log, TEXT("Level Up! New Level: %d, XP: %f/%f"), CurrentLevel, CurrentXP, RequiredXP);
}

void UBDAttributeSet::UpdateMaxHealth()
{
	int32 CurrentLevel = FMath::FloorToInt(Level.GetCurrentValue());
	CurrentLevel = FMath::Clamp(CurrentLevel, 1, 6);

	// 현재 체력 비율 계산
	float CurrentHealth = Health.GetCurrentValue();
	float OldMaxHealth = MaxHealth.GetCurrentValue();
	float HealthRatio = OldMaxHealth > 0.0f ? CurrentHealth / OldMaxHealth : 1.0f;

	// 새 최대 체력 설정
	float NewMaxHealth = MaxHealthTable[CurrentLevel];
	MaxHealth.SetCurrentValue(NewMaxHealth);
	MaxHealth.SetBaseValue(NewMaxHealth);

	// 현재 체력 비율 유지하며 업데이트
	float NewHealth = NewMaxHealth * HealthRatio;
	Health.SetCurrentValue(NewHealth);
	Health.SetBaseValue(NewHealth);

	// 이벤트 발생
	OnHealthChanged.Broadcast(NewHealth, NewMaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Max Health updated to: %f for level %d"), NewMaxHealth, CurrentLevel);
}

void UBDAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// 기존 코드 유지
	if (Attribute == GetMaxHealthAttribute())
	{
		const float CurrentMax = MaxHealth.GetCurrentValue();
		if (!FMath::IsNearlyZero(CurrentMax))
		{
			const float CurrentHealth = Health.GetCurrentValue();
			const float NewRatio = CurrentHealth / CurrentMax;
			NewValue = FMath::Max(NewValue, 1.0f); // 최소 1로 제한
			Health.SetCurrentValue(NewValue * NewRatio);
		}
	}

	// 레벨 속성 제한
	if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.0f, 6.0f);
	}
}

void UBDAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 체력 속성 처리
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float NewHealth = FMath::Clamp(Health.GetCurrentValue(), 0.0f, MaxHealth.GetCurrentValue());
		Health.SetCurrentValue(NewHealth);
		AActor* OwnerActor = GetOwningActor();
		if (OwnerActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("BD_LOG %s Current Health: %.1f"), *OwnerActor->GetName(), NewHealth);
		}
		// Broadcast health change
		OnHealthChanged.Broadcast(NewHealth, MaxHealth.GetCurrentValue());
		if (NewHealth <= 0.0f)
		{
			// 사망 처리
			if (OwnerActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("BD_LOG %s is dead."), *OwnerActor->GetName());
			}
		}
	}

	// 경험치 속성 처리
	else if (Data.EvaluatedData.Attribute == GetXPAttribute())
	{
		// 경험치가 변경되면 레벨업 확인
		float CurrentXP = XP.GetCurrentValue();
		float RequiredXP = NextLevelXP.GetCurrentValue();

		if (CurrentXP >= RequiredXP && RequiredXP > 0.0f)
		{
			HandleLevelUp();
		}
	}

	// 레벨 속성 처리
	else if (Data.EvaluatedData.Attribute == GetLevelAttribute())
	{
		// 레벨이 직접 변경되면 체력 등을 업데이트
		UpdateMaxHealth();

		int32 CurrentLevel = FMath::FloorToInt(Level.GetCurrentValue());
		float CurrentXP = XP.GetCurrentValue();
		float RequiredXP = NextLevelXP.GetCurrentValue();

		// 레벨 변경 이벤트 발생
		OnLevelChanged.Broadcast(CurrentLevel, CurrentXP, RequiredXP);
	}
}