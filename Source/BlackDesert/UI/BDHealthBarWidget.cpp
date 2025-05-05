// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UBDHealthBarWidget::SetHealthPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}
