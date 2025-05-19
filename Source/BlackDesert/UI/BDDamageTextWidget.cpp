// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDDamageTextWidget.h"
#include "Components/TextBlock.h"

UBDDamageTextWidget::UBDDamageTextWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 초기화
    CurrentDamage = 0.0f;
}

void UBDDamageTextWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 이름으로 위젯 찾기 시도
    UWidget* FoundWidget = GetWidgetFromName(TEXT("DamageText"));
    if (FoundWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("DamageTextLog Found DamageText widget by name!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DamageTextLog Could NOT find DamageText widget by name!"));
    }

    // DamageText 위젯이 있는지 확인
    if (!DamageText)
    {
        UE_LOG(LogTemp, Error, TEXT("DamageTextLog BDDamageTextWidget: DamageText is not assigned!"));
        return;
    }

    // 초기 스타일 설정
    DamageText->SetColorAndOpacity(DamageColor);
    // 폰트 크기 설정
    DamageText->SetFont(FSlateFontInfo(DamageText->GetFont().FontObject, FontSize));
}

void UBDDamageTextWidget::SetDamageValue(float Damage)
{
    UE_LOG(LogTemp, Warning, TEXT("DamageTextLog SetDamageValue() %f"), Damage);


    if (!DamageText)
    {
        UE_LOG(LogTemp, Warning, TEXT("DamageTextLog BDDamageTextWidget: DamageText is not assigned"));
        return;
    }

    // 데미지 값 저장
    CurrentDamage = Damage;

    // 데미지 값을 정수로 표시 (소수점 없이)
    FString DamageString = FString::Printf(TEXT("%d"), FMath::RoundToInt(Damage));

    // 텍스트 설정
    DamageText->SetText(FText::FromString(DamageString));
    // 텍스트 색상 설정
    DamageText->SetColorAndOpacity(DamageColor);
}