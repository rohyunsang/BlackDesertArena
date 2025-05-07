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
    // �浹 ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* CollisionComponent;

    // �޽� ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* MeshComponent;

    // ������ ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemID;

    // ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity = 1;

    // ������ ȸ�� ȿ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    bool bShouldRotate = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float RotationRate = 45.0f;

    // ������ ���� ȿ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    bool bShouldFloat = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float FloatHeight = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float FloatSpeed = 2.0f;

    // ������ �̺�Ʈ ó�� �Լ�
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    // ȸ�� �� ���� ȿ���� ���� Tick �Լ�
    virtual void Tick(float DeltaTime) override;

private:
    // ���� ȿ���� ���� ����
    float InitialZ;
    float RunningTime;
};
