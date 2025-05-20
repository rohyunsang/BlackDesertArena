// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDMagicFieldWidget.h"
#include "Actor/BDSafeZoneActor.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

const FLinearColor UBDMagicFieldWidget::CurrentPhaseColor = FLinearColor(0.0f, 0.498f, 1.0f, 1.0f); // 007FFFFF
const FLinearColor UBDMagicFieldWidget::NextPhaseWaitColor = FLinearColor(1.0f, 0.165f, 0.188f, 1.0f); // FF2A30FF

void UBDMagicFieldWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 초기화
    if (PhaseText)
        PhaseText->SetText(FText::FromString(""));

    if (TimeRemainingText)
        TimeRemainingText->SetText(FText::FromString(""));

    if (PhaseProgressBar)
    {
        PhaseProgressBar->SetPercent(0.0f);
        PhaseProgressBar->SetFillColorAndOpacity(CurrentPhaseColor);
    }

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

    // 현재 활성화된 페이즈와 게임 시간 가져오기
    int32 ActivePhaseIndex = SafeZoneActor->ActivePhaseIndex;
    float GameTimeSeconds = SafeZoneActor->GameTimeSeconds;

    if (ActivePhaseIndex >= 0 && ActivePhaseIndex < SafeZoneActor->PhaseTable.Num())
    {
        const FZonePhase& CurrentPhase = SafeZoneActor->PhaseTable[ActivePhaseIndex];

        // 현재 페이즈 정보 업데이트
        if (PhaseText)
        {
            PhaseText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
                ActivePhaseIndex + 1)));
        }

        // 남은 시간 계산
        float TimeRemaining = FMath::Max(0.0f, CurrentPhase.EndTime - GameTimeSeconds);

        // 현재 페이즈의 남은 시간이 0인 경우, 다음 페이즈 대기 시간 처리
        if (TimeRemaining <= 0.0f)
        {
            // 다음 페이즈가 있는지 확인
            int32 NextPhaseIndex = ActivePhaseIndex + 1;
            if (NextPhaseIndex < SafeZoneActor->PhaseTable.Num())
            {
                // 다음 페이즈까지 남은 시간 계산
                const FZonePhase& NextPhase = SafeZoneActor->PhaseTable[NextPhaseIndex];
                float NextPhaseWaitTime = NextPhase.StartTime - GameTimeSeconds;
                float WaitPhaseDuration = NextPhase.StartTime - CurrentPhase.EndTime;

                if (NextPhaseWaitTime > 0.0f)
                {
                    // 다음 페이즈까지 대기 시간 표시 (텍스트)
                    if (TimeRemainingText)
                    {
                        TimeRemainingText->SetText(FText::FromString(FString::Printf(
                            TEXT("%s"), *FormatTime(NextPhaseWaitTime))));
                    }

                    // 진행바에도 다음 페이즈 대기 시간 반영
                    if (PhaseProgressBar)
                    {
                        // 대기 시간의 진행도 계산 (0: 대기 시작, 1: 대기 종료)
                        float WaitProgress = 1.0f - (NextPhaseWaitTime / WaitPhaseDuration);
                        PhaseProgressBar->SetPercent(WaitProgress);

                        // 대기 시간에는 빨간색 계열 사용 (FF2A30FF)
                        PhaseProgressBar->SetFillColorAndOpacity(NextPhaseWaitColor);
                    }
                }
                else
                {
                    // 다음 페이즈가 이미 시작되었어야 함
                    if (TimeRemainingText)
                    {
                        TimeRemainingText->SetText(FText::FromString("Starting Next Phase..."));
                    }

                    if (PhaseProgressBar)
                    {
                        PhaseProgressBar->SetPercent(1.0f); // 대기 완료
                        // 전환 시점에도 빨간색 유지
                        PhaseProgressBar->SetFillColorAndOpacity(CurrentPhaseColor);
                    }
                }
            }
            else
            {
                // 마지막 페이즈인 경우
                if (TimeRemainingText)
                {
                    TimeRemainingText->SetText(FText::FromString("Final Phase Complete"));
                }

                if (PhaseProgressBar)
                {
                    PhaseProgressBar->SetPercent(1.0f); // 모든 페이즈 완료
                    // 완료 상태에서는 기본 색상 사용
                    PhaseProgressBar->SetFillColorAndOpacity(NextPhaseWaitColor);
                }
            }
        }
        else
        {
            // 기존 코드: 현재 페이즈 남은 시간 표시
            if (TimeRemainingText)
            {
                TimeRemainingText->SetText(FText::FromString(FString::Printf(TEXT("%s"),
                    *FormatTime(TimeRemaining))));
            }

            // 기존 코드: 현재 페이즈 진행도 업데이트
            if (PhaseProgressBar)
            {
                float PhaseDuration = CurrentPhase.EndTime - CurrentPhase.StartTime;
                float ElapsedTime = GameTimeSeconds - CurrentPhase.StartTime;
                PhaseProgressBar->SetPercent(1.0f - (TimeRemaining / PhaseDuration));

                // 현재 페이즈의 남은 시간에는 청록색 계열 사용 (007FFFFF)
                PhaseProgressBar->SetFillColorAndOpacity(CurrentPhaseColor);
            }
        }
    }
    else
    {
        // 활성화된 페이즈가 없는 경우 (기존 코드와 동일)
        if (PhaseText)
            PhaseText->SetText(FText::FromString(""));

        if (TimeRemainingText)
            TimeRemainingText->SetText(FText::FromString(""));

        if (PhaseProgressBar)
        {
            PhaseProgressBar->SetPercent(0.0f);
            // 기본 색상 사용
            PhaseProgressBar->SetFillColorAndOpacity(CurrentPhaseColor);
        }
    }
}


FString UBDMagicFieldWidget::FormatTime(float TimeInSeconds) const
{
    int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
    int32 Seconds = FMath::FloorToInt(TimeInSeconds - Minutes * 60.0f);

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}