// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BDAttributeSet.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Health, float, MaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLevelChanged, int32, NewLevel, float, CurrentXP, float, NextLevelXP);

// ATTRIBUTE_ACCESSORS ��ũ��: �־��� �Ӽ��� ���� ������ �Լ����� �ڵ����� �����մϴ�.
// ClassName�� Ŭ���� �̸�, PropertyName�� �Ӽ� �̸��� ���ڷ� �޽��ϴ�.
// �� ��ũ�δ� �Ӽ��� getter, setter �� �ʱ�ȭ �Լ��� �����մϴ�.
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBDAttributeSet();

	// ü�� �Ӽ�
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, MaxHealth)

		// ���� �ý��� �Ӽ�
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Level")
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, Level)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Level")
	FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, XP)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Level")
	FGameplayAttributeData NextLevelXP;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, NextLevelXP)

public:
	// ü�� ���� �̺�Ʈ
	UPROPERTY()
	FOnHealthChanged OnHealthChanged;

	// ���� ���� �̺�Ʈ
	UPROPERTY()
	FOnLevelChanged OnLevelChanged;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// �ʱ� ���� ����
	void InitializeLevel(int32 StartLevel = 1);

	// ����ġ ȹ�� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Character|Attributes")
	void AddXP(float XPAmount);

	// ������ ó�� �Լ�
	void HandleLevelUp();

	// �ִ� ü�� ������Ʈ �Լ�
	void UpdateMaxHealth();

private:
	// ������ �ʿ� ����ġ ���̺�
	TArray<float> XPRequirementTable;

	// ������ �ִ� ü�� ���̺�
	TArray<float> MaxHealthTable;
	
};
