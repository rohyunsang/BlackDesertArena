// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BDAttributeSet.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Health, float, MaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLevelChanged, int32, NewLevel, float, CurrentXP, float, NextLevelXP);

// ATTRIBUTE_ACCESSORS 매크로: 주어진 속성에 대한 접근자 함수들을 자동으로 생성합니다.
// ClassName은 클래스 이름, PropertyName은 속성 이름을 인자로 받습니다.
// 이 매크로는 속성의 getter, setter 및 초기화 함수를 정의합니다.
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

	// 체력 속성
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBDAttributeSet, MaxHealth)

		// 레벨 시스템 속성
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
	// 체력 변경 이벤트
	UPROPERTY()
	FOnHealthChanged OnHealthChanged;

	// 레벨 변경 이벤트
	UPROPERTY()
	FOnLevelChanged OnLevelChanged;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// 초기 레벨 설정
	void InitializeLevel(int32 StartLevel = 1);

	// 경험치 획득 함수
	UFUNCTION(BlueprintCallable, Category = "Character|Attributes")
	void AddXP(float XPAmount);

	// 레벨업 처리 함수
	void HandleLevelUp();

	// 최대 체력 업데이트 함수
	void UpdateMaxHealth();

private:
	// 레벨별 필요 경험치 테이블
	TArray<float> XPRequirementTable;

	// 레벨별 최대 체력 테이블
	TArray<float> MaxHealthTable;
	
};
