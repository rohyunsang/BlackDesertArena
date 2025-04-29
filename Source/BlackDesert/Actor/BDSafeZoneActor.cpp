// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/BDSafeZoneActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"


ABDSafeZoneActor::ABDSafeZoneActor()
{
    PrimaryActorTick.bCanEverTick = true;

    ZoneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneMesh"));
    ZoneMesh->SetCollisionProfileName("OverlapAll");
    ZoneMesh->SetGenerateOverlapEvents(true);
    RootComponent = ZoneMesh;
}

void ABDSafeZoneActor::BeginPlay()
{
    Super::BeginPlay();

    CurrentCenter = GetActorLocation();
    GameTimeSeconds = 0.f;

    DynMat = ZoneMesh->CreateAndSetMaterialInstanceDynamic(0);

    if (PhaseTable.Num() > 0)
    {
        CurrentRadius = PhaseTable[0].StartRadius;
        UpdateZoneScale();
    }

    GetWorldTimerManager().SetTimer(DamageTickHandle, this, &ABDSafeZoneActor::ApplyDamageTick, 1.f, true);
}

void ABDSafeZoneActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    GameTimeSeconds += DeltaTime;

    for (int32 i = 0; i < PhaseTable.Num(); ++i)
    {
        const FZonePhase& Phase = PhaseTable[i];
        if (GameTimeSeconds >= Phase.StartTime && GameTimeSeconds <= Phase.EndTime)
        {
            ActivePhaseIndex = i;

            const float PhaseAlpha = (GameTimeSeconds - Phase.StartTime) / (Phase.EndTime - Phase.StartTime);
            CurrentRadius = FMath::Lerp(Phase.StartRadius, Phase.EndRadius, PhaseAlpha);

            UpdateZoneScale();
            break;
        }
    }
}

void ABDSafeZoneActor::UpdateZoneScale()
{
    const float Scale = CurrentRadius / 50.f; // 1m 구체 메시 기준
    ZoneMesh->SetWorldScale3D(FVector(Scale));
    SetActorLocation(CurrentCenter);

    if (DynMat)
    {
        DynMat->SetScalarParameterValue(TEXT("Radius"), CurrentRadius);
    }
}

void ABDSafeZoneActor::ApplyDamageTick()
{
    if (!PhaseTable.IsValidIndex(ActivePhaseIndex)) return;

    float DamageAmount = PhaseTable[ActivePhaseIndex].Damage;

    for (TActorIterator<APawn> It(GetWorld()); It; ++It)
    {
        APawn* Pawn = *It;
        if (!IsActorOutside(Pawn)) continue;

        UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
        if (!ASC || !OutZoneDamageGE) continue;

        FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(OutZoneDamageGE, 1.f, Context);

        if (SpecHandle.IsValid())
        {
            //SpecHandle.Data->SetSetByCallerMagnitude(FName("Data.Damage"), -DamageAmount);
            SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), -DamageAmount);
            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
}

bool ABDSafeZoneActor::IsActorOutside(const AActor* Actor) const
{
    return FVector::DistXY(Actor->GetActorLocation(), CurrentCenter) > CurrentRadius;
}
