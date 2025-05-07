// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BDDropItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BlackDesertCharacter.h" // 플레이어 클래스 헤더로 교체
#include "Component/BDInventoryComponent.h"

ABDDropItem::ABDDropItem()
{
    PrimaryActorTick.bCanEverTick = true; // Tick 활성화

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    CollisionComponent->InitSphereRadius(50.f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    RootComponent = CollisionComponent;
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABDDropItem::OnOverlapBegin);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 기본값 설정
    ItemID = "DefaultItem";
    Quantity = 1;
    bShouldRotate = true;
    RotationRate = 45.0f;
    bShouldFloat = true;
    FloatHeight = 20.0f;
    FloatSpeed = 2.0f;
    RunningTime = 0.0f;
}

// Called when the game starts or when spawned
void ABDDropItem::BeginPlay()
{
    Super::BeginPlay();

    // 초기 위치 저장
    InitialZ = GetActorLocation().Z;
}

void ABDDropItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 회전 효과
    if (bShouldRotate)
    {
        FRotator NewRotation = GetActorRotation();
        NewRotation.Yaw += DeltaTime * RotationRate;
        SetActorRotation(NewRotation);
    }

    // 부유 효과
    if (bShouldFloat)
    {
        RunningTime += DeltaTime;

        // 사인 함수로 부드러운 상하 움직임
        float DeltaZ = FMath::Sin(RunningTime * FloatSpeed) * FloatHeight;

        FVector NewLocation = GetActorLocation();
        NewLocation.Z = InitialZ + DeltaZ;
        SetActorLocation(NewLocation);
    }
}

void ABDDropItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        ABlackDesertCharacter* Player = Cast<ABlackDesertCharacter>(OtherActor);
        if (Player && Player->Inventory)
        {
            Player->PickupItem(this);
        }
    }
}