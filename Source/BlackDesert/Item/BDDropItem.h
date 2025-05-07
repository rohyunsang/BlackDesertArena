// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BDDropItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class BLACKDESERT_API ABDDropItem : public AActor
{
	GENERATED_BODY()
	
public:
    // Sets default values for this actor's properties
    ABDDropItem();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // 충돌 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* CollisionComponent;

    // 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* MeshComponent;

    // 아이템 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemID;

    // 아이템 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity = 1;

    // 아이템 회전 효과
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    bool bShouldRotate = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float RotationRate = 45.0f;

    // 아이템 부유 효과
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    bool bShouldFloat = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float FloatHeight = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float FloatSpeed = 2.0f;

    // 오버랩 이벤트 처리 함수
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    // 회전 및 부유 효과를 위한 Tick 함수
    virtual void Tick(float DeltaTime) override;

private:
    // 부유 효과를 위한 변수
    float InitialZ;
    float RunningTime;
};
