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
    // Ŭ���� ���� ��ư��
    UPROPERTY(meta = (BindWidget))
    UButton* FletcherButton;

    UPROPERTY(meta = (BindWidget))
    UButton* TitanButton;

    UPROPERTY(meta = (BindWidget))
    UButton* IgneousButton;

    // ���� ���� ��ư
    UPROPERTY(meta = (BindWidget))
    UButton* StartGameButton;

    // ���õ� Ŭ���� ǥ�ÿ� �ؽ�Ʈ
    UPROPERTY(meta = (BindWidget))
    UTextBlock* SelectedClassText;

    // Ŭ���� �̹�����
    UPROPERTY(meta = (BindWidget))
    UImage* FletcherImage;

    UPROPERTY(meta = (BindWidget))
    UImage* TitanImage;

    UPROPERTY(meta = (BindWidget))
    UImage* IgneousImage;

    // ���� ���õ� Ŭ����
    UPROPERTY(BlueprintReadOnly, Category = "Selection")
    EPlayerClass CurrentSelectedClass;

    // ���� �ν��Ͻ� ����
    UPROPERTY()
    UBDGameInstance* GameInstance;

    // ��ư Ŭ�� �̺�Ʈ �Լ���
    UFUNCTION()
    void OnFletcherButtonClicked();

    UFUNCTION()
    void OnTitanButtonClicked();

    UFUNCTION()
    void OnIgneousButtonClicked();

    UFUNCTION()
    void OnStartGameButtonClicked();

    // Ŭ���� ���� ������Ʈ �Լ�
    UFUNCTION()
    void UpdateClassSelection(EPlayerClass NewClass);

    // UI ������Ʈ �Լ�
    UFUNCTION()
    void UpdateUI();

};
