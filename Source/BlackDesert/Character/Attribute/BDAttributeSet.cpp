// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Attribute/BDAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UBDAttributeSet::UBDAttributeSet()
{
	// 초기값을 여기서 설정하지 않음. 보통 GameplayEffect로 초기화함
}

void UBDAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// MaxHealth가 바뀌면 Health도 비율 유지하면서 갱신할 수 있음 (옵션)
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
			// 사망 처리
			if (OwnerActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("BD_LOG %s is dead."), *OwnerActor->GetName());
			}
		}
	}
}