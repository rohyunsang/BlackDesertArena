// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDHUDUI.h"
#include "Game/BDGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Widget.h"

UBDHUDUI::UBDHUDUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // �ʱ�ȭ
    LastSelectedClass = EPlayerClass::None;
    GameInstanceRef = nullptr;
}

void UBDHUDUI::NativeConstruct()
{
    Super::NativeConstruct();

    // ���� �ν��Ͻ� ��������
    GameInstanceRef = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    // �ʱ� ���� ����
    if (GameInstanceRef)
    {
        LastSelectedClass = GameInstanceRef->GetSelectedClass();
        UpdateSkillPanels();
    }
}


void UBDHUDUI::UpdateSkillPanels()
{
    // �г��� ��� �����ϴ��� Ȯ��
    if (!FletcherSkillPanel || !IgneousSkillPanel || !TitanSkillPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("BDHUDUI: One or more skill panels are missing!"));
        return;
    }

    // ���õ� Ŭ������ ���� �г� Ȱ��ȭ/��Ȱ��ȭ
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

    default: // None �Ǵ� ��ȿ���� ���� Ŭ����
        // �⺻������ ��� �г� ����
        FletcherSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        IgneousSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        TitanSkillPanel->SetVisibility(ESlateVisibility::Collapsed);
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("BDHUDUI: Updated skill panels for class: %d"), (int32)LastSelectedClass);
}