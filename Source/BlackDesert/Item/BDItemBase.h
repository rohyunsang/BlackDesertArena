// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BDItemBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BLACKDESERT_API UBDItemBase : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStack = 99;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;
};
