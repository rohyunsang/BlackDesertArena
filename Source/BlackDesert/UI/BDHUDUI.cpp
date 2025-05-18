// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDHUDUI.h"
#include "Game/BDGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Widget.h"

UBDHUDUI::UBDHUDUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 초기화
    LastSelectedClass = EPlayerClass::None;
    GameInstanceRef = nullptr;
}

void UBDHUDUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 게임 인스턴스 가져오기
    GameInstanceRef = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    // 초기 상태 설정
    if (GameInstanceRef)
    {
        LastSelectedClass = GameInstanceRef->GetSelectedClass();
        UpdateSkillPanels();
    }
}


void UBDHUDUI::UpdateSkillPanels()
{
    // 패널이 모두 존재하는지 확인
    if (!FletcherSkillPanel || !IgneousSkillPanel || !TitanSkillPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("BDHUDUI: One or more skill panels are missing!"));
        return;
    }

    // 선택된 클래스에 따라 패널 활성화/비활성화
    switch (LastSelectedClass)
    {
    case EPlayerClass::Fletcher:
        FletcherSkillPanel->SetVisibility(ESlateVisibility::Visible);
        IgneousSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        TitanSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        break;

    case EPlayerClass::Igneous:
        FletcherSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        IgneousSkillPanel->SetVisibility(ESlateVisibility::Visible);
        TitanSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        break;

    case EPlayerClass::Titan:
        FletcherSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        IgneousSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        TitanSkillPanel->SetVisibility(ESlateVisibility::Visible);
        break;

    default: // None 또는 유효하지 않은 클래스
        // 기본적으로 모든 패널 숨김
        FletcherSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        IgneousSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        TitanSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("BDHUDUI: Updated skill panels for class: %d"), (int32)LastSelectedClass);
}