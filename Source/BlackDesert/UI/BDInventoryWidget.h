// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/BDInventoryComponent.h"
#include "BDInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
    void InitInventory(class UBDInventoryComponent* InInventoryComponent);
    void UpdateInventory();
    void AddItemToSlot(const FString& ItemID, int32 Quantity);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UUniformGridPanel* SlotGrid;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UProgressBar* WeightBar;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* WeightText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TSubclassOf<class UBDInventorySlotWidget> SlotWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 GridColumns = 5;

public:
    UPROPERTY()
    class UBDInventoryComponent* InventoryComponent;

    UPROPERTY()
    TArray<class UBDInventorySlotWidget*> SlotWidgets;

    UFUNCTION()
    void OnInventoryUpdated(const TArray<FInventoryItem>& Items);

    UFUNCTION()
    void OnItemAdded(const FInventoryItem& AddedItem);

public:
    // ������ �߰� �� �ش� ���Ը� ������Ʈ
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItemAndUpdateSlot(const FString& ItemID, int32 Quantity = 1);

public:
    // ���� �ʱ�ȭ �Լ� �߰�
    void InitializeSlots();
};
