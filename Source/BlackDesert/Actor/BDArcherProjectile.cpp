// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/BDArcherProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


ABDArcherProjectile::ABDArcherProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision Component
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	RootComponent = CollisionComp;
	CollisionComp->OnComponentHit.AddDynamic(this, &ABDArcherProjectile::OnHit);

	// Mesh Component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.5f;

	InitialLifeSpan = 5.0f;
}

void ABDArcherProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (TrailVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailVFX,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
}

void ABDArcherProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABDArcherProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		UGameplayStatics::ApplyPointDamage(
			OtherActor,
			Damage,
			GetActorForwardVector(),
			Hit,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
	}

	Destroy();
}
