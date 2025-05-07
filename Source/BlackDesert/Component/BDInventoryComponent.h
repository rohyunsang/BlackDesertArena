// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "BDInventoryComponent.generated.h"

class UBDItemBase;

// 아이템 데이터 테이블 구조체
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

// 인벤토리 아이템 구조체
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

// 결과 열거형
UENUM(BlueprintType)
enum class EInventoryResult : uint8
{
    Success,
    Failed_InventoryFull,
    Failed_InvalidItem,
    Failed_NotEnoughItems
};

// 인벤토리 업데이트 델리게이트
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
    // 인벤토리 아이템 배열
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<FInventoryItem> Items;

    // 최대 슬롯 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxSlots;

    // 최대 무게
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    float MaxWeight;

    // 현재 무게
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    float CurrentWeight;

    // 아이템 데이터 테이블
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    UDataTable* ItemDataTable;

    // 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnInventoryUpdated OnInventoryUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnItemAdded OnItemAdded;

    // ID로 아이템 추가
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult AddItemByID(const FString& ItemID, int32 Quantity = 1);

    // 아이템 제거
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    EInventoryResult RemoveItemByID(const FString& ItemID, int32 Quantity = 1);

    // 인벤토리 조회 함수들
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FBDItemData GetItemData(const FString& ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 FindItemIndex(const FString& ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool HasSpaceForItem(const FString& ItemID, int32 Quantity = 1) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void PrintInventory() const;
};
