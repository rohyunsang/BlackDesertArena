// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "BDInventoryComponent.generated.h"

class UBDItemBase;

// ������ ������ ���̺� ����ü
USTRUCT(BlueprintType)
struct FBDItemData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UTexture2D* ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UStaticMesh* ItemMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float Weight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bIsConsumable;

    FBDItemData()
    {
        ItemID = "";
        ItemName = "Default Item";
        MaxStackSize = 99;
        Weight = 1.0f;
        bIsConsumable = false;
    }
};

// �κ��丮 ������ ����ü
USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemID;

    UPROPERTY(BlueprintReadOnly, Category = "Item")
    FString ItemName;

    UPROPERTY(BlueprintReadOnly, Category = "Item")
    UTexture2D* ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity;

    FInventoryItem()
    {
        ItemID = "";
        ItemName = "";
        ItemIcon = nullptr;
        Quantity = 0;
    }
};

// ��� ������
UENUM(BlueprintType)
enum class EInventoryResult : uint8
{
    Success,
    Failed_InventoryFull,
    Failed_InvalidItem,
    Failed_NotEnoughItems
};

// �κ��丮 ������Ʈ ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, const TArray<FInventoryItem>&, Inventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const FInventoryItem&, AddedItem);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLACKDESERT_API UBDInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UBDInventoryComponent();

protected:
    virtual void BeginPlay() override;

public:
    // �κ��丮 ������ �迭
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<FInventoryItem> Items;

    // �ִ� ���� ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxSlots;

    // �ִ� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    float MaxWeight;

    // ���� ����
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    float CurrentWeight;

    // ������ ������ ���̺�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    UDataTable* ItemDataTable;

    // ��������Ʈ
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnInventoryUpdated OnInventoryUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnItemAdded OnItemAdded;

    // ID�� ������ �߰�
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult AddItemByID(const FString& ItemID, int32 Quantity = 1);

    // ������ ����
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult RemoveItemByID(const FString& ItemID, int32 Quantity = 1);

    // �κ��丮 ��ȸ �Լ���
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FBDItemData GetItemData(const FString& ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 FindItemIndex(const FString& ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool HasSpaceForItem(const FString& ItemID, int32 Quantity = 1) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void PrintInventory() const;
};
