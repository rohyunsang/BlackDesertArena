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

    // ������ �� ���� �Լ�
    UFUNCTION(BlueprintCallable, Category = "UI|Damage")
    void SetDamageValue(float Damage);

protected:
    // ������ �ؽ�Ʈ ��� (BP���� �Ҵ�)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* DamageText;

    // ������ ����
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|Damage")
    FSlateColor DamageColor = FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)); 
    // ��Ʈ ũ��
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|Damage")
    float FontSize = 20.0f;

    // ���� ������ ��
    float CurrentDamage;
	
};
