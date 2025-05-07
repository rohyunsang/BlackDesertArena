// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BDMonsterDropComponent.generated.h"

class ABDDropItem;

// ����� ������ ���� ����ü
USTRUCT(BlueprintType)
struct FMonsterDropItemInfo
{
    GENERATED_BODY()

    // ����� ������ �������Ʈ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    TSubclassOf<ABDDropItem> ItemClass;

    // ��� Ȯ�� (0.0 ~ 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DropChance = 0.5f;

    // �ּ� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 MinQuantity = 1;

    // �ִ� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 MaxQuantity = 1;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLACKDESERT_API UBDMonsterDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBDMonsterDropComponent();

protected:
    // ���� ���� �� ȣ��
    virtual void BeginPlay() override;

public:
    // ����� ������ ���
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    TArray<FMonsterDropItemInfo> PossibleDropItems;

    // �ּ� ��� ������ ���� ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 MinDropItemTypes = 0;

    // �ִ� ��� ������ ���� ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 MaxDropItemTypes = 2;

    // ��� �ݰ�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    float DropRadius = 100.0f;

    // ���� ������
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    float VerticalOffset = 50.0f;

    // ������ ��� �Լ� - ���� ��� �� ȣ��
    UFUNCTION(BlueprintCallable, Category = "Drop")
    void DropLoot();

    // Ư�� ������ ����
    UFUNCTION(BlueprintCallable, Category = "Drop")
    ABDDropItem* SpawnDropItem(TSubclassOf<ABDDropItem> ItemClass, int32 Quantity);

    // ��� ��ġ ���
    UFUNCTION(BlueprintCallable, Category = "Drop")
    FVector CalculateDropLocation() const;

		
};
