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
	// ü�� ������Ʈ�� �Լ�
	UFUNCTION(BlueprintCallable)
	void UpdateUI(float Health, float MaxHealth);

protected:
	// ���ε��� ProgressBar ���� (UMG���� BindWidget üũ�ڽ� �ʿ�)
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPText;
};
