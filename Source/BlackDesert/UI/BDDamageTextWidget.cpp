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

    // 흰색(1.0)~회색(0.5) 사이의 랜덤 값 생성
    // 모든 채널(R,G,B)에 동일한 값을 적용하여 흰색~회색 계열 생성
    float RandomGrayValue = FMath::RandRange(0.5f, 1.0f);
    FLinearColor RandomColor = FLinearColor(RandomGrayValue, RandomGrayValue, RandomGrayValue, 1.0f);

    // 랜덤 색상 적용
    DamageText->SetColorAndOpacity(FSlateColor(RandomColor));

    // 랜덤 폰트 크기 생성 (기본 크기 +/- 5)
    float RandomFontSize = FontSize + FMath::RandRange(-5.0f, 5.0f);

    // 최소 크기 보장
    if (RandomFontSize < 15.0f)
    {
        RandomFontSize = 15.0f;
    }

    // 랜덤 폰트 크기 적용
    DamageText->SetFont(FSlateFontInfo(DamageText->GetFont().FontObject, RandomFontSize));

    UE_LOG(LogTemp, Warning, TEXT("DamageTextLog Applied random gray value: %f, random size: %f"), RandomGrayValue, RandomFontSize);
}