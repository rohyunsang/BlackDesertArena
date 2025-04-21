// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Attribute/BDAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UBDAttributeSet::UBDAttributeSet()
{
	// �ʱⰪ�� ���⼭ �������� ����. ���� GameplayEffect�� �ʱ�ȭ��
}

void UBDAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// MaxHealth�� �ٲ�� Health�� ���� �����ϸ鼭 ������ �� ���� (�ɼ�)
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
}

void UBDAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float NewHealth = FMath::Clamp(Health.GetCurrentValue(), 0.0f, MaxHealth.GetCurrentValue());
		Health.SetCurrentValue(NewHealth);

		AActor* OwnerActor = GetOwningActor();
		if (OwnerActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("BD_LOG %s Current Health: %.1f"), *OwnerActor->GetName(), NewHealth);
		}

		if (NewHealth <= 0.0f)
		{
			// ��� ó��
			if (OwnerActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("BD_LOG %s is dead."), *OwnerActor->GetName());
			}
		}
	}
}