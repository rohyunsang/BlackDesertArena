// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBDHealthBarWidget::UpdateUI(float Health, float MaxHealth, int CurLevel)
{
	if (HealthProgressBar && MaxHealth > 0.0f)
	{
		const float Percent = Health / MaxHealth;
		HealthProgressBar->SetPercent(Percent);
	}

	if (HPText)
	{
		const FString HPString = FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth);
		HPText->SetText(FText::FromString(HPString));
	}

	if (LevelText)
	{
		int32 CurLevel = 5; // 예시 레벨 값
		LevelText->SetText(FText::FromString(FString::Printf(TEXT("Lv. %d"), CurLevel)));
	}
}