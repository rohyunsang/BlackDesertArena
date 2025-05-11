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

    // ���� �ν��Ͻ� ��������
    GameInstance = Cast<UBDGameInstance>(GetGameInstance());

    // ��ư Ŭ�� �̺�Ʈ ���ε�
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

    // �ʱ� ���� ����
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
        // Ŭ������ ���õ��� �ʾ��� �� ��� �޽���
        if (SelectedClassText)
        {
            SelectedClassText->SetText(FText::FromString(TEXT("Please select a class first!")));
        }
        return;
    }

    // ���� �ν��Ͻ��� ������ Ŭ���� ����
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

    // ���� �ν��Ͻ����� ����
    if (GameInstance)
    {
        GameInstance->SetSelectedClass(CurrentSelectedClass);
    }
}

void UBDClassSelectionWidget::UpdateUI()
{
    // ���õ� Ŭ���� �ؽ�Ʈ ������Ʈ
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

    // �̹������� ������ ��Ÿ�� ���� (���õ� Ŭ���� ���̶���Ʈ)
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

    // ���� ��ư Ȱ��ȭ/��Ȱ��ȭ
    if (StartGameButton)
    {
        StartGameButton->SetIsEnabled(CurrentSelectedClass != EPlayerClass::None);
    }
}