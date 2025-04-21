// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BDAttributeSet.generated.h"

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

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, MaxHealth)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
};
