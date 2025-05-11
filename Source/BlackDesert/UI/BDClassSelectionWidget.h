// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/BDGameInstance.h"
#include "BDClassSelectionWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDClassSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    // 클래스 선택 버튼들
    UPROPERTY(meta = (BindWidget))
    UButton* FletcherButton;

    UPROPERTY(meta = (BindWidget))
    UButton* TitanButton;

    UPROPERTY(meta = (BindWidget))
    UButton* IgneousButton;

    // 게임 시작 버튼
    UPROPERTY(meta = (BindWidget))
    UButton* StartGameButton;

    // 선택된 클래스 표시용 텍스트
    UPROPERTY(meta = (BindWidget))
    UTextBlock* SelectedClassText;

    // 클래스 이미지들
    UPROPERTY(meta = (BindWidget))
    UImage* FletcherImage;

    UPROPERTY(meta = (BindWidget))
    UImage* TitanImage;

    UPROPERTY(meta = (BindWidget))
    UImage* IgneousImage;

    // 현재 선택된 클래스
    UPROPERTY(BlueprintReadOnly, Category = "Selection")
    EPlayerClass CurrentSelectedClass;

    // 게임 인스턴스 참조
    UPROPERTY()
    UBDGameInstance* GameInstance;

    // 버튼 클릭 이벤트 함수들
    UFUNCTION()
    void OnFletcherButtonClicked();

    UFUNCTION()
    void OnTitanButtonClicked();

    UFUNCTION()
    void OnIgneousButtonClicked();

    UFUNCTION()
    void OnStartGameButtonClicked();

    // 클래스 선택 업데이트 함수
    UFUNCTION()
    void UpdateClassSelection(EPlayerClass NewClass);

    // UI 업데이트 함수
    UFUNCTION()
    void UpdateUI();

};
