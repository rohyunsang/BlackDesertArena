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
    // 생성자
    UBDHUDUI(const FObjectInitializer& ObjectInitializer);

    // 위젯 초기화 시 호출
    virtual void NativeConstruct() override;

protected:
    // 각 클래스별 스킬 패널
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* FletcherSkillPanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* IgneousSkillPanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* TitanSkillPanel;

    // 현재 선택된 클래스
    EPlayerClass LastSelectedClass;

    // 클래스 변경에 따라 UI 업데이트
    void UpdateSkillPanels();

    // 게임 인스턴스 참조 캐싱
    UPROPERTY()
    UBDGameInstance* GameInstanceRef;
	
};
