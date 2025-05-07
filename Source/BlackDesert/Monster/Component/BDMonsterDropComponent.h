// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BDMonsterDropComponent.generated.h"

class ABDDropItem;

// 드롭할 아이템 정보 구조체
USTRUCT(BlueprintType)
struct FMonsterDropItemInfo
{
    GENERATED_BODY()

    // 드롭할 아이템 블루프린트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    TSubclassOf<ABDDropItem> ItemClass;

    // 드롭 확률 (0.0 ~ 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DropChance = 0.5f;

    // 최소 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 MinQuantity = 1;

    // 최대 수량
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
    // 게임 시작 시 호출
    virtual void BeginPlay() override;

public:
    // 드롭할 아이템 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    TArray<FMonsterDropItemInfo> PossibleDropItems;

    // 최소 드롭 아이템 종류 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 MinDropItemTypes = 0;

    // 최대 드롭 아이템 종류 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 MaxDropItemTypes = 2;

    // 드롭 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    float DropRadius = 100.0f;

    // 수직 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    float VerticalOffset = 50.0f;

    // 아이템 드롭 함수 - 몬스터 사망 시 호출
    UFUNCTION(BlueprintCallable, Category = "Drop")
    void DropLoot();

    // 특정 아이템 스폰
    UFUNCTION(BlueprintCallable, Category = "Drop")
    ABDDropItem* SpawnDropItem(TSubclassOf<ABDDropItem> ItemClass, int32 Quantity);

    // 드롭 위치 계산
    UFUNCTION(BlueprintCallable, Category = "Drop")
    FVector CalculateDropLocation() const;

		
};
