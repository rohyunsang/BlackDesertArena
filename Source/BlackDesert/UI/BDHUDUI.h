// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/BDGameInstance.h"
#include "BDHUDUI.generated.h"

class UWidget;

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDHUDUI : public UUserWidget
{
	GENERATED_BODY()

public:
    // ������
    UBDHUDUI(const FObjectInitializer& ObjectInitializer);

    // ���� �ʱ�ȭ �� ȣ��
    virtual void NativeConstruct() override;

protected:
    // �� Ŭ������ ��ų �г�
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* FletcherSkillPanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* IgneousSkillPanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* TitanSkillPanel;

    // ���� ���õ� Ŭ����
    EPlayerClass LastSelectedClass;

    // Ŭ���� ���濡 ���� UI ������Ʈ
    void UpdateSkillPanels();

    // ���� �ν��Ͻ� ���� ĳ��
    UPROPERTY()
    UBDGameInstance* GameInstanceRef;
	
};
