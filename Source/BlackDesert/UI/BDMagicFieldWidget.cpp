// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDMagicFieldWidget.h"
#include "Actor/BDSafeZoneActor.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UBDMagicFieldWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // �ʱ�ȭ
    if (PhaseText)
        PhaseText->SetText(FText::FromString("Phase: --"));

    if (TimeRemainingText)
        TimeRemainingText->SetText(FText::FromString("Time: --:--"));

    if (PhaseProgressBar)
        PhaseProgressBar->SetPercent(0.0f);

    if (CurrentRadiusText)
        CurrentRadiusText->SetText(FText::FromString("Current: -- m"));

    if (NextRadiusText)
        NextRadiusText->SetText(FText::FromString("Next: -- m"));
}

void UBDMagicFieldWidget::Init(ABDSafeZoneActor* InSafeZoneActor)
{
    SafeZoneActor = InSafeZoneActor;
}

void UBDMagicFieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateUI();
}

void UBDMagicFieldWidget::UpdateUI()
{
    if (!SafeZoneActor) return;

    // ���� Ȱ��ȭ�� ������� ���� �ð� ��������
    int32 ActivePhaseIndex = SafeZoneActor->ActivePhaseIndex;
    float GameTimeSeconds = SafeZoneActor->GameTimeSeconds;

    if (ActivePhaseIndex >= 0 && ActivePhaseIndex < SafeZoneActor->PhaseTable.Num())
    {
        const FZonePhase& CurrentPhase = SafeZoneActor->PhaseTable[ActivePhaseIndex];

        // ���� ������ ���� ������Ʈ
        if (PhaseText)
        {
            PhaseText->SetText(FText::FromString(FString::Printf(TEXT("Phase: %d/%d"),
                ActivePhaseIndex + 1, SafeZoneActor->PhaseTable.Num())));
        }

        // ���� �ð� ��� �� ������Ʈ
        float TimeRemaining = FMath::Max(0.0f, CurrentPhase.EndTime - GameTimeSeconds);
        if (TimeRemainingText)
        {
            TimeRemainingText->SetText(FText::FromString(FString::Printf(TEXT("Time: %s"),
                *FormatTime(TimeRemaining))));
        }

        // ����� ������Ʈ
        if (PhaseProgressBar)
        {
            float PhaseDuration = CurrentPhase.EndTime - CurrentPhase.StartTime;
            float ElapsedTime = GameTimeSeconds - CurrentPhase.StartTime;
            PhaseProgressBar->SetPercent(1.0f - (TimeRemaining / PhaseDuration));
        }

        // ���� �ݰ� �� ���� �ݰ� ���� ������Ʈ
        if (CurrentRadiusText)
        {
            CurrentRadiusText->SetText(FText::FromString(FString::Printf(TEXT("Current: %.0f m"),
                SafeZoneActor->CurrentRadius / 100.0f))); // �𸮾� ������ ���ͷ� ��ȯ
        }

        if (NextRadiusText)
        {
            NextRadiusText->SetText(FText::FromString(FString::Printf(TEXT("Next: %.0f m"),
                CurrentPhase.EndRadius / 100.0f))); // �𸮾� ������ ���ͷ� ��ȯ
        }
    }
    else
    {
        // Ȱ��ȭ�� ����� ���� ���
        if (PhaseText)
            PhaseText->SetText(FText::FromString("Phase: --"));

        if (TimeRemainingText)
            TimeRemainingText->SetText(FText::FromString("Time: --:--"));

        if (PhaseProgressBar)
            PhaseProgressBar->SetPercent(0.0f);

        if (CurrentRadiusText)
            CurrentRadiusText->SetText(FText::FromString("Current: -- m"));

        if (NextRadiusText)
            NextRadiusText->SetText(FText::FromString("Next: -- m"));
    }
}

FString UBDMagicFieldWidget::FormatTime(float TimeInSeconds) const
{
    int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
    int32 Seconds = FMath::FloorToInt(TimeInSeconds - Minutes * 60.0f);

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}