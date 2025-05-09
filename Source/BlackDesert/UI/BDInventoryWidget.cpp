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

    // �ʱ� ���� UI ���� (���� �Լ��� �и�)
    InitializeSlots();

    // ���� ǥ�� ���� �˻�
    if (!WeightBar)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG NativeConstruct: WeightBar widget not found! Check UMG bindings"));
    }

    if (!WeightText)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG NativeConstruct: WeightText widget not found! Check UMG bindings"));
    }

    // ���� �׸��� �˻�
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
        // ��������Ʈ ����
        InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UBDInventoryWidget::OnInventoryUpdated);
        InventoryComponent->OnItemAdded.AddDynamic(this, &UBDInventoryWidget::OnItemAdded);

        // ������ �ʱ�ȭ���� �ʾҴٸ� �ʱ�ȭ
        if (SlotWidgets.Num() == 0)
        {
            UE_LOG(LogTemp, Log, TEXT("BD_LOG InitInventory: No slots, initializing"));
            InitializeSlots();
        }

        // �ʱ� �κ��丮 ���·� UI ������Ʈ
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

    // ���� ���� �ʱ�ȭ
    SlotGrid->ClearChildren();
    SlotWidgets.Empty();

    // �÷��̾� ��Ʈ�ѷ� ��������
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG InitializeSlots: No PlayerController"));
        return;
    }

    // �κ��丮 ������Ʈ�� ���� ĳ���Ϳ��� �ִ� ���� �� ��������
    int32 MaxSlots = 20; // �⺻��
    if (InventoryComponent)
    {
        MaxSlots = InventoryComponent->MaxSlots;
        UE_LOG(LogTemp, Log, TEXT("BD_LOG InitializeSlots: Using MaxSlots=%d from component"), MaxSlots);
    }

    // �׸��� ���� ����
    for (int32 i = 0; i < MaxSlots; i++)
    {
        UBDInventorySlotWidget* SlotWidget = CreateWidget<UBDInventorySlotWidget>(PC, SlotWidgetClass);
        if (SlotWidget)
        {
            int32 Row = i / GridColumns;
            int32 Column = i % GridColumns;

            SlotWidget->SlotIndex = i;
            SlotWidget->ClearItemData(); // �ʱ� ���´� �������

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

    // ������ ����ִٸ� �ʱ�ȭ �õ�
    if (SlotWidgets.Num() == 0 && SlotGrid && SlotWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BD_LOG UpdateInventory: No slots, attempting to initialize"));
        InitializeSlots();
    }

    // ��� ���� �ʱ�ȭ
    for (UBDInventorySlotWidget* SlotWidget : SlotWidgets)
    {
        if (SlotWidget)
        {
            SlotWidget->ClearItemData();
        }
    }

    // �κ��丮 ���������� ���� ä���
    const TArray<FInventoryItem>& Items = InventoryComponent->Items;
    for (int32 i = 0; i < Items.Num() && i < SlotWidgets.Num(); i++)
    {
        UE_LOG(LogTemp, Log, TEXT("BD_LOG UpdateInventory: Setting slot %d with item %s (x%d)"),
            i, *Items[i].ItemName, Items[i].Quantity);

        SlotWidgets[i]->SetItemData(Items[i]);
    }

    // ���� ǥ�� ������Ʈ
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
    // ������ �߰� ȿ�� (�ʿ� �� ����)
    // ��: ���� ���̶���Ʈ ȿ�� ��
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

    // ������ �߰� �� �κ��丮 ���� Ȯ��
    int32 ExistingIndex = InventoryComponent->FindItemIndex(ItemID);
    bool bExistingItem = (ExistingIndex != INDEX_NONE);
    int32 ExistingQuantity = bExistingItem ? InventoryComponent->Items[ExistingIndex].Quantity : 0;

    // �κ��丮�� ������ �߰�
    // EInventoryResult Result = InventoryComponent->AddItemByID(ItemID, Quantity);
    EInventoryResult Result = EInventoryResult::Success;

    if (Result == EInventoryResult::Success)
    {
        if (bExistingItem)
        {
            // ���� �����ۿ� ���õ� ���
            int32 NewIndex = InventoryComponent->FindItemIndex(ItemID);
            if (NewIndex != INDEX_NONE)
            {
                // ���� �������� �ִ� ��� ���� ã�Ƽ� ������Ʈ
                for (int32 i = 0; i < SlotWidgets.Num(); i++)
                {
                    if (SlotWidgets[i] && SlotWidgets[i]->ItemData.ItemID == ItemID)
                    {
                        SlotWidgets[i]->SetItemData(InventoryComponent->Items[NewIndex]);

                        // ���� ������Ʈ ȿ�� ���� ���⼭ �߰��� �� ����
                        break;
                    }
                }
            }
        }
        else
        {
            // �� �������� �߰��� ���, �ش� �������� �� ���Ը� ������Ʈ
            int32 NewIndex = InventoryComponent->FindItemIndex(ItemID);
            if (NewIndex != INDEX_NONE && NewIndex < SlotWidgets.Num())
            {
                SlotWidgets[NewIndex]->SetItemData(InventoryComponent->Items[NewIndex]);

                // �� ������ �߰� ȿ�� ���� ���⼭ ���� ����
            }
            else
            {
                // ������ ��ġ�� ��Ȯ�� Ư���� �� ���� ��� ��ü ������Ʈ
                UpdateInventory();
            }
        }

        // ���� ǥ�� ������Ʈ
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