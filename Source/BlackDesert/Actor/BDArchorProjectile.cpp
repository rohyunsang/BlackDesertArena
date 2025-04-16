// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BDArchorProjectile.h"

// Sets default values
ABDArchorProjectile::ABDArchorProjectile()  // Will Delete
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABDArchorProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABDArchorProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

