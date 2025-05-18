// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Attribute/BDAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UBDAttributeSet::UBDAttributeSet()
{
	// ������ �ʿ� ����ġ ���̺� �ʱ�ȭ
	XPRequirementTable = {
		0.0f,    // ���� 0->1 (������� ����)
		250.0f,  // ���� 1->2
		550.0f,  // ���� 2->3
		950.0f,  // ���� 3->4
		1450.0f, // ���� 4->5
		2050.0f, // ���� 5->6
		0.0f     // ���� 6�� �ִ� ���� (������ ����)
	};

	// ������ �ִ� ü�� ���̺� �ʱ�ȭ
	MaxHealthTable = {
		0.0f,    // ���� 0 (������� ����)
		1000.0f, // ���� 1
		1200.0f, // ���� 2
		1400.0f, // ���� 3
		1600.0f, // ���� 4
		1800.0f, // ���� 5
		2000.0f  // ���� 6
	};

	// �ʱ� ���� �� �Ӽ� ���� (�⺻��, �����÷��� ����Ʈ�� ��� �� ����)
	InitializeLevel(1);
}

void UBDAttributeSet::InitializeLevel(int32 StartLevel)
{
	// ��ȿ�� ���� ���� (1~6) Ȯ��
	StartLevel = FMath::Clamp(StartLevel, 1, 6);

	// ���� ����
	Level.SetCurrentValue(StartLevel);
	Level.SetBaseValue(StartLevel);

	// ����ġ �ʱ�ȭ
	XP.SetCurrentValue(0.0f);
	XP.SetBaseValue(0.0f);

	// ���� ���� �ʿ� ����ġ ����
	if (StartLevel < 6)
	{
		NextLevelXP.SetCurrentValue(XPRequirementTable[StartLevel]);
		NextLevelXP.SetBaseValue(XPRequirementTable[StartLevel]);
	}
	else
	{
		// �ִ� ������ ���
		NextLevelXP.SetCurrentValue(0.0f);
		NextLevelXP.SetBaseValue(0.0f);
	}

	// �ִ� ü�� ����
	MaxHealth.SetCurrentValue(MaxHealthTable[StartLevel]);
	MaxHealth.SetBaseValue(MaxHealthTable[StartLevel]);

	// ���� ü�� ���� (�ִ� ü�°� �����ϰ� ����)
	Health.SetCurrentValue(MaxHealth.GetCurrentValue());
	Health.SetBaseValue(MaxHealth.GetCurrentValue());

	// �̺�Ʈ �߻�
	OnLevelChanged.Broadcast(StartLevel, XP.GetCurrentValue(), NextLevelXP.GetCurrentValue());
	OnHealthChanged.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());
}

void UBDAttributeSet::AddXP(float XPAmount)
{
	// �̹� �ִ� �����̸� ����ġ ȹ�� �Ұ�
	int32 CurrentLevel = FMath::FloorToInt(Level.GetCurrentValue());
	if (CurrentLevel >= 6)
	{
		return;
	}

	// ����ġ �߰�
	float CurrentXP = XP.GetCurrentValue();
	float NewXP = CurrentXP + XPAmount;
	XP.SetCurrentValue(NewXP);
	XP.SetBaseValue(NewXP);

	// ������ ���� Ȯ��
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

	// �̹� �ִ� �����̸� ������ �Ұ�
	if (CurrentLevel >= 6)
	{
		return;
	}

	// ���� ����ġ�� �ʿ� ����ġ
	float CurrentXP = XP.GetCurrentValue();
	float RequiredXP = NextLevelXP.GetCurrentValue();

	// ������ �������� Ȯ��
	while (CurrentXP >= RequiredXP && CurrentLevel < 6)
	{
		// ���� ����
		CurrentLevel++;

		// ���� ����ġ ���
		CurrentXP -= RequiredXP;

		// ���� ���� �ʿ� ����ġ ����
		if (CurrentLevel < 6)
		{
			RequiredXP = XPRequirementTable[CurrentLevel];
		}
		else
		{
			// �ִ� ������ ����
			RequiredXP = 0.0f;
			CurrentXP = 0.0f;
		}
	}

	// ���� ����
	Level.SetCurrentValue(CurrentLevel);
	Level.SetBaseValue(CurrentLevel);

	// ����ġ ������Ʈ
	XP.SetCurrentValue(CurrentXP);
	XP.SetBaseValue(CurrentXP);

	// ���� ���� �ʿ� ����ġ ������Ʈ
	NextLevelXP.SetCurrentValue(RequiredXP);
	NextLevelXP.SetBaseValue(RequiredXP);

	// �ִ� ü�� ������Ʈ
	UpdateMaxHealth();

	// �̺�Ʈ �߻�
	OnLevelChanged.Broadcast(CurrentLevel, CurrentXP, RequiredXP);

	UE_LOG(LogTemp, Log, TEXT("Level Up! New Level: %d, XP: %f/%f"), CurrentLevel, CurrentXP, RequiredXP);
}

void UBDAttributeSet::UpdateMaxHealth()
{
	int32 CurrentLevel = FMath::FloorToInt(Level.GetCurrentValue());
	CurrentLevel = FMath::Clamp(CurrentLevel, 1, 6);

	// ���� ü�� ���� ���
	float CurrentHealth = Health.GetCurrentValue();
	float OldMaxHealth = MaxHealth.GetCurrentValue();
	float HealthRatio = OldMaxHealth > 0.0f ? CurrentHealth / OldMaxHealth : 1.0f;

	// �� �ִ� ü�� ����
	float NewMaxHealth = MaxHealthTable[CurrentLevel];
	MaxHealth.SetCurrentValue(NewMaxHealth);
	MaxHealth.SetBaseValue(NewMaxHealth);

	// ���� ü�� ���� �����ϸ� ������Ʈ
	float NewHealth = NewMaxHealth * HealthRatio;
	Health.SetCurrentValue(NewHealth);
	Health.SetBaseValue(NewHealth);

	// �̺�Ʈ �߻�
	OnHealthChanged.Broadcast(NewHealth, NewMaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Max Health updated to: %f for level %d"), NewMaxHealth, CurrentLevel);
}

void UBDAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// ���� �ڵ� ����
	if (Attribute == GetMaxHealthAttribute())
	{
		const float CurrentMax = MaxHealth.GetCurrentValue();
		if (!FMath::IsNearlyZero(CurrentMax))
		{
			const float CurrentHealth = Health.GetCurrentValue();
			const float NewRatio = CurrentHealth / CurrentMax;
			NewValue = FMath::Max(NewValue, 1.0f); // �ּ� 1�� ����
			Health.SetCurrentValue(NewValue * NewRatio);
		}
	}

	// ���� �Ӽ� ����
	if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.0f, 6.0f);
	}
}

void UBDAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// ü�� �Ӽ� ó��
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
			// ��� ó��
			if (OwnerActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("BD_LOG %s is dead."), *OwnerActor->GetName());
			}
		}
	}

	// ����ġ �Ӽ� ó��
	else if (Data.EvaluatedData.Attribute == GetXPAttribute())
	{
		// ����ġ�� ����Ǹ� ������ Ȯ��
		float CurrentXP = XP.GetCurrentValue();
		float RequiredXP = NextLevelXP.GetCurrentValue();

		if (CurrentXP >= RequiredXP && RequiredXP > 0.0f)
		{
			HandleLevelUp();
		}
	}

	// ���� �Ӽ� ó��
	else if (Data.EvaluatedData.Attribute == GetLevelAttribute())
	{
		// ������ ���� ����Ǹ� ü�� ���� ������Ʈ
		UpdateMaxHealth();

		int32 CurrentLevel = FMath::FloorToInt(Level.GetCurrentValue());
		float CurrentXP = XP.GetCurrentValue();
		float RequiredXP = NextLevelXP.GetCurrentValue();

		// ���� ���� �̺�Ʈ �߻�
		OnLevelChanged.Broadcast(CurrentLevel, CurrentXP, RequiredXP);
	}
}