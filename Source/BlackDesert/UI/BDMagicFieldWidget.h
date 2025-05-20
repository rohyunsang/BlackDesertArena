// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BDMagicFieldWidget.generated.h"

class UTextBlock;
class UProgressBar;
class ABDSafeZoneActor;

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDMagicFieldWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void Init(ABDSafeZoneActor* InSafeZoneActor);
    void UpdateUI();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PhaseText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimeRemainingText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* PhaseProgressBar;

private:
    UPROPERTY()
    ABDSafeZoneActor* SafeZoneActor;

    FString FormatTime(float TimeInSeconds) const;


public:
    // ���� ����
    static const FLinearColor CurrentPhaseColor;    // 007FFFFF - û�ϻ� (���� ������)
    static const FLinearColor NextPhaseWaitColor;   // FF2A30FF - ������ (���� ������ ���)
};
