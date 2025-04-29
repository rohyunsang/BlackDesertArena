// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "BDSafeZoneActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FZonePhase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartTime = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EndTime = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartRadius = 30000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EndRadius = 20000.f;
};


UCLASS()
class BLACKDESERT_API ABDSafeZoneActor : public AActor
{
    GENERATED_BODY()

public:
    ABDSafeZoneActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Phase")
    TArray<FZonePhase> PhaseTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> OutZoneDamageGE;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* ZoneMesh;

private:
    int32 ActivePhaseIndex = -1;
    float GameTimeSeconds = 0.f;
    float CurrentRadius = 0.f;
    FVector CurrentCenter;

    UMaterialInstanceDynamic* DynMat = nullptr;
    FTimerHandle DamageTickHandle;

    void UpdateZoneScale();
    void ApplyDamageTick();
    bool IsActorOutside(const AActor* Actor) const;
};