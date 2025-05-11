// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDClassSelectionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UBDClassSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 게임 인스턴스 가져오기
    GameInstance = Cast<UBDGameInstance>(GetGameInstance());

    // 버튼 클릭 이벤트 바인딩
    if (FletcherButton)
    {
        FletcherButton->OnClicked.AddDynamic(this, &UBDClassSelectionWidget::OnFletcherButtonClicked);
    }

    if (TitanButton)
    {
        TitanButton->OnClicked.AddDynamic(this, &UBDClassSelectionWidget::OnTitanButtonClicked);
    }

    if (IgneousButton)
    {
        IgneousButton->OnClicked.AddDynamic(this, &UBDClassSelectionWidget::OnIgneousButtonClicked);
    }

    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &UBDClassSelectionWidget::OnStartGameButtonClicked);
    }

    // 초기 상태 설정
    CurrentSelectedClass = EPlayerClass::None;
    UpdateUI();
}

void UBDClassSelectionWidget::OnFletcherButtonClicked()
{
    UpdateClassSelection(EPlayerClass::Fletcher);
}

void UBDClassSelectionWidget::OnTitanButtonClicked()
{
    UpdateClassSelection(EPlayerClass::Titan);
}

void UBDClassSelectionWidget::OnIgneousButtonClicked()
{
    UpdateClassSelection(EPlayerClass::Igneous);
}

void UBDClassSelectionWidget::OnStartGameButtonClicked()
{
    if (CurrentSelectedClass == EPlayerClass::None)
    {
        // 클래스가 선택되지 않았을 때 경고 메시지
        if (SelectedClassText)
        {
            SelectedClassText->SetText(FText::FromString(TEXT("Please select a class first!")));
        }
        return;
    }

    // 게임 인스턴스에 선택한 클래스 저장
    if (GameInstance)
    {
        GameInstance->SetSelectedClass(CurrentSelectedClass);
        GameInstance->LoadGameLevel("Main");
    }
}

void UBDClassSelectionWidget::UpdateClassSelection(EPlayerClass NewClass)
{
    CurrentSelectedClass = NewClass;
    UpdateUI();

    // 게임 인스턴스에도 저장
    if (GameInstance)
    {
        GameInstance->SetSelectedClass(CurrentSelectedClass);
    }
}

void UBDClassSelectionWidget::UpdateUI()
{
    // 선택된 클래스 텍스트 업데이트
    if (SelectedClassText)
    {
        FString ClassText;
        switch (CurrentSelectedClass)
        {
        case EPlayerClass::Fletcher:
            ClassText = TEXT("Selected: Fletcher");
            break;
        case EPlayerClass::Titan:
            ClassText = TEXT("Selected: Titan");
            break;
        case EPlayerClass::Igneous:
            ClassText = TEXT("Selected: Igneous");
            break;
        default:
            ClassText = TEXT("Select a Class");
            break;
        }
        SelectedClassText->SetText(FText::FromString(ClassText));
    }

    // 이미지들의 투명도나 스타일 조정 (선택된 클래스 하이라이트)
    if (FletcherImage)
    {
        FletcherImage->SetOpacity(CurrentSelectedClass == EPlayerClass::Fletcher ? 1.0f : 0.5f);
    }
    if (TitanImage)
    {
        TitanImage->SetOpacity(CurrentSelectedClass == EPlayerClass::Titan ? 1.0f : 0.5f);
    }
    if (IgneousImage)
    {
        IgneousImage->SetOpacity(CurrentSelectedClass == EPlayerClass::Igneous ? 1.0f : 0.5f);
    }

    // 시작 버튼 활성화/비활성화
    if (StartGameButton)
    {
        StartGameButton->SetIsEnabled(CurrentSelectedClass != EPlayerClass::None);
    }
}