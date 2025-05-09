// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/BDInventoryComponent.h"
#include "BDInventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void SetItemData(const FInventoryItem& InItemData);
    void ClearItemData();

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UImage* ItemIcon;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* QuantityText;

    UPROPERTY(BlueprintReadOnly)
    FInventoryItem ItemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    UTexture2D* EmptySlotTexture;

    int32 SlotIndex;

    // 드래그 앤 드롭 관련 함수들
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

};
