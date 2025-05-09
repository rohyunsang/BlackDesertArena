// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BDInventoryWidget.h"
#include "UI/BDInventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Component/BDInventoryComponent.h"

void UBDInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Log, TEXT("BD_LOG InventoryWidget NativeConstruct"));

    // 초기 슬롯 UI 설정 (별도 함수로 분리)
    InitializeSlots();

    // 무게 표시 위젯 검사
    if (!WeightBar)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG NativeConstruct: WeightBar widget not found! Check UMG bindings"));
    }

    if (!WeightText)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG NativeConstruct: WeightText widget not found! Check UMG bindings"));
    }

    // 슬롯 그리드 검사
    if (!SlotGrid)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG NativeConstruct: SlotGrid widget not found! Check UMG bindings"));
    }

    if (!SlotWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG NativeConstruct: SlotWidgetClass not set! Set it in Blueprint"));
    }
}

void UBDInventoryWidget::InitInventory(UBDInventoryComponent* InInventoryComponent)
{
    UE_LOG(LogTemp, Log, TEXT("BD_LOG InitInventory called with component: %s"),
        InInventoryComponent ? TEXT("valid") : TEXT("NULL"));

    InventoryComponent = InInventoryComponent;

    if (InventoryComponent)
    {
        // 델리게이트 연결
        InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UBDInventoryWidget::OnInventoryUpdated);
        InventoryComponent->OnItemAdded.AddDynamic(this, &UBDInventoryWidget::OnItemAdded);

        // 슬롯이 초기화되지 않았다면 초기화
        if (SlotWidgets.Num() == 0)
        {
            UE_LOG(LogTemp, Log, TEXT("BD_LOG InitInventory: No slots, initializing"));
            InitializeSlots();
        }

        // 초기 인벤토리 상태로 UI 업데이트
        UpdateInventory();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG InitInventory: No InventoryComponent provided"));
    }
}

void UBDInventoryWidget::InitializeSlots()
{
    if (!SlotGrid || !SlotWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG InitializeSlots: Missing SlotGrid or SlotWidgetClass"));
        return;
    }

    // 기존 슬롯 초기화
    SlotGrid->ClearChildren();
    SlotWidgets.Empty();

    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG InitializeSlots: No PlayerController"));
        return;
    }

    // 인벤토리 컴포넌트를 가진 캐릭터에서 최대 슬롯 수 가져오기
    int32 MaxSlots = 20; // 기본값
    if (InventoryComponent)
    {
        MaxSlots = InventoryComponent->MaxSlots;
        UE_LOG(LogTemp, Log, TEXT("BD_LOG InitializeSlots: Using MaxSlots=%d from component"), MaxSlots);
    }

    // 그리드 슬롯 생성
    for (int32 i = 0; i < MaxSlots; i++)
    {
        UBDInventorySlotWidget* SlotWidget = CreateWidget<UBDInventorySlotWidget>(PC, SlotWidgetClass);
        if (SlotWidget)
        {
            int32 Row = i / GridColumns;
            int32 Column = i % GridColumns;

            SlotWidget->SlotIndex = i;
            SlotWidget->ClearItemData(); // 초기 상태는 비어있음

            SlotGrid->AddChildToUniformGrid(SlotWidget, Row, Column);
            SlotWidgets.Add(SlotWidget);

            UE_LOG(LogTemp, Verbose, TEXT("BD_LOG InitializeSlots: Created slot %d at [%d,%d]"),
                i, Row, Column);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BD_LOG InitializeSlots: Failed to create slot widget %d"), i);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("BD_LOG InitializeSlots: Created %d slot widgets"), SlotWidgets.Num());
}

void UBDInventoryWidget::UpdateInventory()
{
    if (!InventoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG UpdateInventory: No InventoryComponent!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("BD_LOG UpdateInventory: Items: %d, Slots: %d"),
        InventoryComponent->Items.Num(), SlotWidgets.Num());

    // 슬롯이 비어있다면 초기화 시도
    if (SlotWidgets.Num() == 0 && SlotGrid && SlotWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG UpdateInventory: No slots, attempting to initialize"));
        InitializeSlots();
    }

    // 모든 슬롯 초기화
    for (UBDInventorySlotWidget* SlotWidget : SlotWidgets)
    {
        if (SlotWidget)
        {
            SlotWidget->ClearItemData();
        }
    }

    // 인벤토리 아이템으로 슬롯 채우기
    const TArray<FInventoryItem>& Items = InventoryComponent->Items;
    for (int32 i = 0; i < Items.Num() && i < SlotWidgets.Num(); i++)
    {
        UE_LOG(LogTemp, Log, TEXT("BD_LOG UpdateInventory: Setting slot %d with item %s (x%d)"),
            i, *Items[i].ItemName, Items[i].Quantity);

        SlotWidgets[i]->SetItemData(Items[i]);
    }

    // 무게 표시 업데이트
    if (WeightBar)
    {
        float WeightRatio = InventoryComponent->CurrentWeight / FMath::Max(1.0f, InventoryComponent->MaxWeight);
        WeightBar->SetPercent(WeightRatio);

        UE_LOG(LogTemp, Log, TEXT("BD_LOG UpdateInventory: Weight: %.1f/%.1f (%.1f%%)"),
            InventoryComponent->CurrentWeight, InventoryComponent->MaxWeight, WeightRatio * 100.0f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG UpdateInventory: No WeightBar found! Check UMG bindings"));
    }

    if (WeightText)
    {
        FText WeightFormatText = FText::Format(
            FText::FromString("{0}/{1}"),
            FText::AsNumber(FMath::RoundToInt(InventoryComponent->CurrentWeight)),
            FText::AsNumber(FMath::RoundToInt(InventoryComponent->MaxWeight))
        );

        WeightText->SetText(WeightFormatText);
       
        UE_LOG(LogTemp, Log, TEXT("BD_LOG UpdateInventory: Weight text set to: %s"),
            *WeightFormatText.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG UpdateInventory: No WeightText found! Check UMG bindings"));
    }
}


void UBDInventoryWidget::OnInventoryUpdated(const TArray<FInventoryItem>& Items)
{
    UpdateInventory();
}

void UBDInventoryWidget::OnItemAdded(const FInventoryItem& AddedItem)
{
    // 아이템 추가 효과 (필요 시 구현)
    // 예: 슬롯 하이라이트 효과 등
}

void UBDInventoryWidget::AddItemToSlot(const FString& ItemID, int32 Quantity)
{
    if (InventoryComponent)
    {
        InventoryComponent->AddItemByID(ItemID, Quantity);
    }
}

bool UBDInventoryWidget::AddItemAndUpdateSlot(const FString& ItemID, int32 Quantity)
{
    if (!InventoryComponent)
    {
        return false;
    }

    // 아이템 추가 전 인벤토리 상태 확인
    int32 ExistingIndex = InventoryComponent->FindItemIndex(ItemID);
    bool bExistingItem = (ExistingIndex != INDEX_NONE);
    int32 ExistingQuantity = bExistingItem ? InventoryComponent->Items[ExistingIndex].Quantity : 0;

    // 인벤토리에 아이템 추가
    // EInventoryResult Result = InventoryComponent->AddItemByID(ItemID, Quantity);
    EInventoryResult Result = EInventoryResult::Success;

    if (Result == EInventoryResult::Success)
    {
        if (bExistingItem)
        {
            // 기존 아이템에 스택된 경우
            int32 NewIndex = InventoryComponent->FindItemIndex(ItemID);
            if (NewIndex != INDEX_NONE)
            {
                // 같은 아이템이 있는 모든 슬롯 찾아서 업데이트
                for (int32 i = 0; i < SlotWidgets.Num(); i++)
                {
                    if (SlotWidgets[i] && SlotWidgets[i]->ItemData.ItemID == ItemID)
                    {
                        SlotWidgets[i]->SetItemData(InventoryComponent->Items[NewIndex]);

                        // 수량 업데이트 효과 등을 여기서 추가할 수 있음
                        break;
                    }
                }
            }
        }
        else
        {
            // 새 아이템이 추가된 경우, 해당 아이템이 들어간 슬롯만 업데이트
            int32 NewIndex = InventoryComponent->FindItemIndex(ItemID);
            if (NewIndex != INDEX_NONE && NewIndex < SlotWidgets.Num())
            {
                SlotWidgets[NewIndex]->SetItemData(InventoryComponent->Items[NewIndex]);

                // 새 아이템 추가 효과 등을 여기서 구현 가능
            }
            else
            {
                // 아이템 위치를 정확히 특정할 수 없는 경우 전체 업데이트
                UpdateInventory();
            }
        }

        // 무게 표시 업데이트
        if (WeightBar)
        {
            float WeightRatio = InventoryComponent->CurrentWeight / FMath::Max(1.0f, InventoryComponent->MaxWeight);
            WeightBar->SetPercent(WeightRatio);
        }

        if (WeightText)
        {
            FText WeightFormatText = FText::Format(
                FText::FromString("{0}/{1}"),
                FText::AsNumber(FMath::RoundToInt(InventoryComponent->CurrentWeight)),
                FText::AsNumber(FMath::RoundToInt(InventoryComponent->MaxWeight))
            );
            WeightText->SetText(WeightFormatText);
        }

        return true;
    }

    return false;
}