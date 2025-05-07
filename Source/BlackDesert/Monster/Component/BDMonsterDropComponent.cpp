// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Component/BDMonsterDropComponent.h"
#include "Item/BDDropItem.h"
#include "Kismet/KismetMathLibrary.h"
UBDMonsterDropComponent::UBDMonsterDropComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 기본값 설정
    MinDropItemTypes = 0;
    MaxDropItemTypes = 2;
    DropRadius = 100.0f;
    VerticalOffset = -50.0f;
}

void UBDMonsterDropComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UBDMonsterDropComponent::DropLoot()
{
    // 드롭할 아이템이 없으면 종료
    if (PossibleDropItems.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG: No items to drop"));
        return;
    }

    // 드롭할 아이템 목록 복사 및 섞기
    TArray<FMonsterDropItemInfo> ShuffledItems = PossibleDropItems;
    for (int32 i = ShuffledItems.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        ShuffledItems.Swap(i, j);
    }

    // 드롭할 아이템 종류 수 결정
    int32 NumToDrop = FMath::RandRange(MinDropItemTypes, FMath::Min(MaxDropItemTypes, ShuffledItems.Num()));
    int32 DroppedCount = 0;

    UE_LOG(LogTemp, Log, TEXT("BD_LOG: Attempting to drop %d item types"), NumToDrop);

    // 각 아이템에 대해 드롭 확률에 따라 처리
    for (const FMonsterDropItemInfo& ItemInfo : ShuffledItems)
    {
        if (DroppedCount >= NumToDrop)
        {
            break;
        }

        if (!ItemInfo.ItemClass)
        {
            continue;
        }

        float RandValue = FMath::FRand(); // 0.0 ~ 1.0

        if (RandValue <= ItemInfo.DropChance)
        {
            // 드롭 수량 결정
            int32 Quantity = FMath::RandRange(ItemInfo.MinQuantity, ItemInfo.MaxQuantity);

            // 아이템 스폰
            ABDDropItem* DroppedItem = SpawnDropItem(ItemInfo.ItemClass, Quantity);

            if (DroppedItem)
            {
                DroppedCount++;
                UE_LOG(LogTemp, Log, TEXT("BD_LOG: Dropped item %s x%d"),
                    *ItemInfo.ItemClass->GetName(), Quantity);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("BD_LOG: Actually dropped %d item types"), DroppedCount);
}

ABDDropItem* UBDMonsterDropComponent::SpawnDropItem(TSubclassOf<ABDDropItem> ItemClass, int32 Quantity)
{
    if (!ItemClass || Quantity <= 0)
    {
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    // 드롭 위치 계산
    FVector DropLocation = CalculateDropLocation();

    // 아이템 스폰
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ABDDropItem* DroppedItem = World->SpawnActor<ABDDropItem>(ItemClass, DropLocation, FRotator::ZeroRotator, SpawnParams);

    if (DroppedItem)
    {
        // 수량 설정 (필요한 경우)
        DroppedItem->Quantity = Quantity;
    }

    return DroppedItem;
}

FVector UBDMonsterDropComponent::CalculateDropLocation() const
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return FVector::ZeroVector;
    }

    FVector OwnerLocation = Owner->GetActorLocation();

    // 랜덤한 방향으로 드롭 위치 계산
    float RandomAngle = FMath::FRand() * 360.0f;
    float RandomDistance = FMath::FRand() * DropRadius;

    FVector Offset = FVector(
        RandomDistance * FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
        RandomDistance * FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
        VerticalOffset
    );

    return OwnerLocation + Offset;
}