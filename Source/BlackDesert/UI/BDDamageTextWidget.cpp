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

    // ���(1.0)~ȸ��(0.5) ������ ���� �� ����
    // ��� ä��(R,G,B)�� ������ ���� �����Ͽ� ���~ȸ�� �迭 ����
    float RandomGrayValue = FMath::RandRange(0.5f, 1.0f);
    FLinearColor RandomColor = FLinearColor(RandomGrayValue, RandomGrayValue, RandomGrayValue, 1.0f);

    // ���� ���� ����
    DamageText->SetColorAndOpacity(FSlateColor(RandomColor));

    // ���� ��Ʈ ũ�� ���� (�⺻ ũ�� +/- 5)
    float RandomFontSize = FontSize + FMath::RandRange(-5.0f, 5.0f);

    // �ּ� ũ�� ����
    if (RandomFontSize < 15.0f)
    {
        RandomFontSize = 15.0f;
    }

    // ���� ��Ʈ ũ�� ����
    DamageText->SetFont(FSlateFontInfo(DamageText->GetFont().FontObject, RandomFontSize));

    UE_LOG(LogTemp, Warning, TEXT("DamageTextLog Applied random gray value: %f, random size: %f"), RandomGrayValue, RandomFontSize);
}