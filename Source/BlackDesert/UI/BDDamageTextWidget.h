// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "BDDamageTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UBDDamageTextWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    // 데미지 값 설정 함수
    UFUNCTION(BlueprintCallable, Category = "UI|Damage")
    void SetDamageValue(float Damage);

protected:
    // 데미지 텍스트 블록 (BP에서 할당)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* DamageText;

    // 데미지 색상
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|Damage")
    FSlateColor DamageColor = FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)); 
    // 폰트 크기
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|Damage")
    float FontSize = 20.0f;

    // 현재 데미지 값
    float CurrentDamage;
	
};
