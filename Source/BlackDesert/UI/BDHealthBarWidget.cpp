// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBDHealthBarWidget::UpdateUI(float Health, float MaxHealth)
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
}