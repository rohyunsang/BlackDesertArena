// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BDHealthBarWidget.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 체력 업데이트용 함수
	UFUNCTION(BlueprintCallable)
	void UpdateUI(float Health, float MaxHealth);

protected:
	// 바인딩할 ProgressBar 변수 (UMG에서 BindWidget 체크박스 필요)
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPText;
};
