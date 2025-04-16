// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BDArchorProjectile.generated.h"

UCLASS()
class BLACKDESERT_API ABDArchorProjectile : public AActor    // Will Delete
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABDArchorProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
