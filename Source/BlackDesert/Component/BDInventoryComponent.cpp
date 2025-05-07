// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/BDInventoryComponent.h"
#include "Item/BDItemBase.h"

UBDInventoryComponent::UBDInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // �⺻�� ����
    MaxSlots = 20;
    MaxWeight = 100.0f;
    CurrentWeight = 0.0f;
}

void UBDInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

EInventoryResult UBDInventoryComponent::AddItemByID(const FString& ItemID, int32 Quantity)
{
    if (ItemID.IsEmpty() || Quantity <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Invalid item ID or quantity"));
        return EInventoryResult::Failed_InvalidItem;
    }

    // ������ ������ ��������
    FBDItemData ItemData = GetItemData(ItemID);
    if (ItemData.ItemID.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Item not found in data table: %s"), *ItemID);
        return EInventoryResult::Failed_InvalidItem;
    }

    // �κ��丮 ���� üũ
    float AddedWeight = ItemData.Weight * Quantity;
    if (CurrentWeight + AddedWeight > MaxWeight)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Inventory too heavy to add %s x%d"), *ItemID, Quantity);
        return EInventoryResult::Failed_InventoryFull;
    }

    // ���� ������ ���ÿ� �߰��� �� �ִ��� Ȯ��
    int32 ExistingIndex = FindItemIndex(ItemID);
    if (ExistingIndex != INDEX_NONE)
    {
        // ���� ���� ���� Ȯ��
        if (Items[ExistingIndex].Quantity + Quantity <= ItemData.MaxStackSize)
        {
            Items[ExistingIndex].Quantity += Quantity;
            CurrentWeight += AddedWeight;

            UE_LOG(LogTemp, Log, TEXT("BD_LOG Added %s x%d to existing stack. New quantity: %d"),
                *ItemID, Quantity, Items[ExistingIndex].Quantity);

            OnInventoryUpdated.Broadcast(Items);
            OnItemAdded.Broadcast(Items[ExistingIndex]);

            return EInventoryResult::Success;
        }
    }

    // �� ���Կ� �߰�
    if (Items.Num() >= MaxSlots)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG Inventory full, can't add new item %s"), *ItemID);
        return EInventoryResult::Failed_InventoryFull;
    }

    // �� ������ ����
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

    // ������ ������ ��������
    FBDItemData ItemData = GetItemData(ItemID);

    // ������ ����
    Items[ItemIndex].Quantity -= Quantity;
    CurrentWeight -= (ItemData.Weight * Quantity);

    // ������ 0�̸� ���Կ��� ����
    if (Items[ItemIndex].Quantity <= 0)
    {
        Items.RemoveAt(ItemIndex);
    }

    UE_LOG(LogTemp, Log, TEXT("BD_LOG Removed %s x%d from inventory"), *ItemID, Quantity);

    OnInventoryUpdated.Broadcast(Items);

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

    // ������ ������ ��������
    FBDItemData ItemData = GetItemData(ItemID);
    if (ItemData.ItemID.IsEmpty())
    {
        return false;
    }

    // ���� üũ
    float AddedWeight = ItemData.Weight * Quantity;
    if (CurrentWeight + AddedWeight > MaxWeight)
    {
        return false;
    }

    // ���� ���ÿ� �߰��� �� �ִ��� Ȯ��
    int32 ExistingIndex = FindItemIndex(ItemID);
    if (ExistingIndex != INDEX_NONE)
    {
        if (Items[ExistingIndex].Quantity + Quantity <= ItemData.MaxStackSize)
        {
            return true;
        }
    }

    // �� ������ �ʿ��� ��� ���� ���� üũ
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