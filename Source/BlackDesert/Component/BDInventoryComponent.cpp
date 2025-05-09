// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/BDInventoryComponent.h"
#include "Item/BDItemBase.h"
#include "UI/BDInventoryWidget.h"
#include "GameFramework/PlayerController.h"
UBDInventoryComponent::UBDInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 기본값 설정
    MaxSlots = 20;
    MaxWeight = 100.0f;
    CurrentWeight = 0.0f;
}

void UBDInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // 초기화 시 PlayerController 가져오기
    APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
    if (PC && InventoryWidgetClass)
    {
        // 위젯 미리 생성하고 초기화 (표시는 하지 않음)
        CreateInventoryWidget(PC);
    }
}

EInventoryResult UBDInventoryComponent::AddItemByID(const FString& ItemID, int32 Quantity)
{
    if (ItemID.IsEmpty() || Quantity <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Invalid item ID or quantity"));
        return EInventoryResult::Failed_InvalidItem;
    }

    // 아이템 데이터 가져오기
    FBDItemData ItemData = GetItemData(ItemID);
    if (ItemData.ItemID.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Item not found in data table: %s"), *ItemID);
        return EInventoryResult::Failed_InvalidItem;
    }

    // 인벤토리 무게 체크
    float AddedWeight = ItemData.Weight * Quantity;
    if (CurrentWeight + AddedWeight > MaxWeight)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Inventory too heavy to add %s x%d"), *ItemID, Quantity);
        return EInventoryResult::Failed_InventoryFull;
    }

    // 기존 아이템 스택에 추가할 수 있는지 확인
    int32 ExistingIndex = FindItemIndex(ItemID);
    if (ExistingIndex != INDEX_NONE)
    {
        // 스택 가능 여부 확인
        if (Items[ExistingIndex].Quantity + Quantity <= ItemData.MaxStackSize)
        {
            Items[ExistingIndex].Quantity += Quantity;
            CurrentWeight += AddedWeight;

            UE_LOG(LogTemp, Log, TEXT("BD_LOG Added %s x%d to existing stack. New quantity: %d"),
                *ItemID, Quantity, Items[ExistingIndex].Quantity);

            OnInventoryUpdated.Broadcast(Items);
            OnItemAdded.Broadcast(Items[ExistingIndex]);

            // 아이템 획득 이벤트 처리
            OnItemPickedUp(ItemID, Quantity);

            return EInventoryResult::Success;
        }
    }

    // 새 슬롯에 추가
    if (Items.Num() >= MaxSlots)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Inventory full, can't add new item %s"), *ItemID);
        return EInventoryResult::Failed_InventoryFull;
    }

    // 새 아이템 생성
    FInventoryItem NewItem;
    NewItem.ItemID = ItemID;
    NewItem.ItemName = ItemData.ItemName;
    NewItem.ItemIcon = ItemData.ItemIcon;
    NewItem.Quantity = Quantity;

    Items.Add(NewItem);
    CurrentWeight += AddedWeight;

    UE_LOG(LogTemp, Log, TEXT("BD_LOG Added new item %s x%d to inventory"), *ItemID, Quantity);

    OnInventoryUpdated.Broadcast(Items);
    OnItemAdded.Broadcast(NewItem);

    // 아이템 획득 이벤트 처리
    OnItemPickedUp(ItemID, Quantity);

    return EInventoryResult::Success;
}

EInventoryResult UBDInventoryComponent::RemoveItemByID(const FString& ItemID, int32 Quantity)
{
    if (ItemID.IsEmpty() || Quantity <= 0)
    {
        return EInventoryResult::Failed_InvalidItem;
    }

    int32 ItemIndex = FindItemIndex(ItemID);
    if (ItemIndex == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Item %s not found in inventory"), *ItemID);
        return EInventoryResult::Failed_NotEnoughItems;
    }

    if (Items[ItemIndex].Quantity < Quantity)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Not enough items. Have %d, trying to remove %d"),
            Items[ItemIndex].Quantity, Quantity);
        return EInventoryResult::Failed_NotEnoughItems;
    }

    // 아이템 데이터 가져오기
    FBDItemData ItemData = GetItemData(ItemID);

    // 아이템 제거
    Items[ItemIndex].Quantity -= Quantity;
    CurrentWeight -= (ItemData.Weight * Quantity);

    // 수량이 0이면 슬롯에서 제거
    if (Items[ItemIndex].Quantity <= 0)
    {
        Items.RemoveAt(ItemIndex);
    }

    UE_LOG(LogTemp, Log, TEXT("BD_LOG Removed %s x%d from inventory"), *ItemID, Quantity);

    OnInventoryUpdated.Broadcast(Items);

    // UI 업데이트
    UpdateInventoryUI();

    return EInventoryResult::Success;
}

FBDItemData UBDInventoryComponent::GetItemData(const FString& ItemID) const
{
    FBDItemData EmptyData;

    if (!ItemDataTable || ItemID.IsEmpty())
    {
        return EmptyData;
    }

    TArray<FName> RowNames = ItemDataTable->GetRowNames();

    for (const FName& RowName : RowNames)
    {
        FBDItemData* FoundData = ItemDataTable->FindRow<FBDItemData>(RowName, "");
        if (FoundData && FoundData->ItemID == ItemID)
        {
            return *FoundData;
        }
    }

    return EmptyData;
}

int32 UBDInventoryComponent::FindItemIndex(const FString& ItemID) const
{
    for (int32 i = 0; i < Items.Num(); i++)
    {
        if (Items[i].ItemID == ItemID)
        {
            return i;
        }
    }

    return INDEX_NONE;
}

bool UBDInventoryComponent::HasSpaceForItem(const FString& ItemID, int32 Quantity) const
{
    if (ItemID.IsEmpty() || Quantity <= 0)
    {
        return false;
    }

    // 아이템 데이터 가져오기
    FBDItemData ItemData = GetItemData(ItemID);
    if (ItemData.ItemID.IsEmpty())
    {
        return false;
    }

    // 무게 체크
    float AddedWeight = ItemData.Weight * Quantity;
    if (CurrentWeight + AddedWeight > MaxWeight)
    {
        return false;
    }

    // 기존 스택에 추가할 수 있는지 확인
    int32 ExistingIndex = FindItemIndex(ItemID);
    if (ExistingIndex != INDEX_NONE)
    {
        if (Items[ExistingIndex].Quantity + Quantity <= ItemData.MaxStackSize)
        {
            return true;
        }
    }

    // 새 슬롯이 필요한 경우 여유 공간 체크
    return Items.Num() < MaxSlots;
}

void UBDInventoryComponent::PrintInventory() const
{
    UE_LOG(LogTemp, Log, TEXT("BD_LOG Inventory Contents (%d/%d items):"), Items.Num(), MaxSlots);

    for (int32 i = 0; i < Items.Num(); i++)
    {
        UE_LOG(LogTemp, Log, TEXT("BD_LOG Slot %d: %s x%d"),
            i, *Items[i].ItemName, Items[i].Quantity);
    }

    UE_LOG(LogTemp, Log, TEXT("BD_LOG Total Weight: %.1f/%.1f"), CurrentWeight, MaxWeight);
}

// UI 관련 함수 구현
UBDInventoryWidget* UBDInventoryComponent::CreateInventoryWidget(APlayerController* PC)
{
    if (!PC || !InventoryWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG CreateInventoryWidget: Missing PlayerController or InventoryWidgetClass"));
        return nullptr;
    }

    if (!InventoryWidget)
    {
        UE_LOG(LogTemp, Log, TEXT("BD_LOG CreateInventoryWidget: Creating new inventory widget"));
        InventoryWidget = CreateWidget<UBDInventoryWidget>(PC, InventoryWidgetClass);
        if (InventoryWidget)
        {
            InventoryWidget->InitInventory(this);
        }
    }

    return InventoryWidget;
}

void UBDInventoryComponent::ToggleInventoryUI()
{
    UE_LOG(LogTemp, Log, TEXT("BD_LOG ToggleInventoryUI called"));

    if (!InventoryWidget)
    {
        APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
        if (!PC)
        {
            UE_LOG(LogTemp, Warning, TEXT("BD_LOG ToggleInventoryUI: No PlayerController"));
            return;
        }

        CreateInventoryWidget(PC);
    }

    if (InventoryWidget)
    {
        if (InventoryWidget->IsInViewport())
        {
            UE_LOG(LogTemp, Log, TEXT("BD_LOG ToggleInventoryUI: Hiding inventory"));
            InventoryWidget->RemoveFromViewport();
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("BD_LOG ToggleInventoryUI: Showing inventory"));
            InventoryWidget->AddToViewport();
            UpdateInventoryUI();
        }
    }
}

void UBDInventoryComponent::UpdateInventoryUI()
{
    if (InventoryWidget)
    {
        UE_LOG(LogTemp, Log, TEXT("BD_LOG UpdateInventoryUI: Updating inventory UI"));

        // 슬롯이 제대로 초기화되었는지 확인
        if (InventoryWidget->SlotWidgets.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("BD_LOG UpdateInventoryUI: No slots, attempting to initialize"));
            InventoryWidget->InitializeSlots();
        }

        // 무게 위젯을 확인
        if (!InventoryWidget->WeightBar || !InventoryWidget->WeightText)
        {
            UE_LOG(LogTemp, Warning, TEXT("BD_LOG UpdateInventoryUI: Missing weight widgets in inventory UI"));
        }

        // 인벤토리 업데이트
        InventoryWidget->UpdateInventory();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG UpdateInventoryUI: No inventory widget available"));
    }
}

void UBDInventoryComponent::OnItemPickedUp(const FString& ItemID, int32 Quantity)
{
    UE_LOG(LogTemp, Log, TEXT("BD_LOG OnItemPickedUp: Item %s x%d"), *ItemID, Quantity);

    // 아이템 정보 가져오기
    FString ItemName;
    UTexture2D* ItemIcon = nullptr;
    GetItemNameAndIcon(ItemID, ItemName, ItemIcon);

    // UI 업데이트 (열려있거나 생성되어 있는 경우에만)
    if (InventoryWidget)
    {
        // 특정 슬롯만 효율적으로 업데이트
        if (InventoryWidget->IsInViewport())
        {
            bool bUpdated = InventoryWidget->AddItemAndUpdateSlot(ItemID, Quantity);
            if (!bUpdated)
            {
                UpdateInventoryUI();
            }
        }
    }

    // 로그 출력
    UE_LOG(LogTemp, Log, TEXT("BD_LOG OnItemPickedUp: %s picked up"), *ItemName);
}

void UBDInventoryComponent::GetItemNameAndIcon(const FString& ItemID, FString& OutName, UTexture2D*& OutIcon)
{
    FBDItemData ItemData = GetItemData(ItemID);
    if (!ItemData.ItemID.IsEmpty())
    {
        OutName = ItemData.ItemName;
        OutIcon = ItemData.ItemIcon;
    }
    else
    {
        OutName = FString::Printf(TEXT("Unknown Item (%s)"), *ItemID);
        OutIcon = nullptr;
    }
}