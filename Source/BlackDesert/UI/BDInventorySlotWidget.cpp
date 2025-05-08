// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDInventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBDInventorySlotWidget::SetItemData(const FInventoryItem& InItemData)
{
    ItemData = InItemData;

    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(InItemData.ItemIcon, true);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }

    if (QuantityText)
    {
        if (InItemData.Quantity > 1)
        {
            QuantityText->SetText(FText::AsNumber(InItemData.Quantity));
            QuantityText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            QuantityText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UBDInventorySlotWidget::ClearItemData()
{
    ItemData = FInventoryItem();

    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(EmptySlotTexture);
    }

    if (QuantityText)
    {
        QuantityText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

FReply UBDInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && !ItemData.ItemID.IsEmpty())
    {
        return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }
    return FReply::Handled();
}

void UBDInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    // 드래그 앤 드롭 코드 구현 (필요시)
}