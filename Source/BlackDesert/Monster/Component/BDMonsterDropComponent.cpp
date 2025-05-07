// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Component/BDMonsterDropComponent.h"
#include "Item/BDDropItem.h"
#include "Kismet/KismetMathLibrary.h"
UBDMonsterDropComponent::UBDMonsterDropComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // �⺻�� ����
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
    // ����� �������� ������ ����
    if (PossibleDropItems.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG: No items to drop"));
        return;
    }

    // ����� ������ ��� ���� �� ����
    TArray<FMonsterDropItemInfo> ShuffledItems = PossibleDropItems;
    for (int32 i = ShuffledItems.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        ShuffledItems.Swap(i, j);
    }

    // ����� ������ ���� �� ����
    int32 NumToDrop = FMath::RandRange(MinDropItemTypes, FMath::Min(MaxDropItemTypes, ShuffledItems.Num()));
    int32 DroppedCount = 0;

    UE_LOG(LogTemp, Log, TEXT("BD_LOG: Attempting to drop %d item types"), NumToDrop);

    // �� �����ۿ� ���� ��� Ȯ���� ���� ó��
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
            // ��� ���� ����
            int32 Quantity = FMath::RandRange(ItemInfo.MinQuantity, ItemInfo.MaxQuantity);

            // ������ ����
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

    // ��� ��ġ ���
    FVector DropLocation = CalculateDropLocation();

    // ������ ����
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ABDDropItem* DroppedItem = World->SpawnActor<ABDDropItem>(ItemClass, DropLocation, FRotator::ZeroRotator, SpawnParams);

    if (DroppedItem)
    {
        // ���� ���� (�ʿ��� ���)
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

    // ������ �������� ��� ��ġ ���
    float RandomAngle = FMath::FRand() * 360.0f;
    float RandomDistance = FMath::FRand() * DropRadius;

    FVector Offset = FVector(
        RandomDistance * FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
        RandomDistance * FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
        VerticalOffset
    );

    return OwnerLocation + Offset;
}