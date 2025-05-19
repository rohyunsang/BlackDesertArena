// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDDamageTextWidget.h"
#include "Components/TextBlock.h"

UBDDamageTextWidget::UBDDamageTextWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // �ʱ�ȭ
    CurrentDamage = 0.0f;
}

void UBDDamageTextWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // �̸����� ���� ã�� �õ�
    UWidget* FoundWidget = GetWidgetFromName(TEXT("DamageText"));
    if (FoundWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("DamageTextLog Found DamageText widget by name!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DamageTextLog Could NOT find DamageText widget by name!"));
    }

    // DamageText ������ �ִ��� Ȯ��
    if (!DamageText)
    {
        UE_LOG(LogTemp, Error, TEXT("DamageTextLog BDDamageTextWidget: DamageText is not assigned!"));
        return;
    }

    // �ʱ� ��Ÿ�� ����
    DamageText->SetColorAndOpacity(DamageColor);
    // ��Ʈ ũ�� ����
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

    // ������ �� ����
    CurrentDamage = Damage;

    // ������ ���� ������ ǥ�� (�Ҽ��� ����)
    FString DamageString = FString::Printf(TEXT("%d"), FMath::RoundToInt(Damage));

    // �ؽ�Ʈ ����
    DamageText->SetText(FText::FromString(DamageString));
    // �ؽ�Ʈ ���� ����
    DamageText->SetColorAndOpacity(DamageColor);
}