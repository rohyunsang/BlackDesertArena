// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BDGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"

ABDGameMode::ABDGameMode()
{
    // 기본 폰 클래스는 사용하지 않음 (선택한 클래스에 따라 결정)
    DefaultPawnClass = nullptr;
}

void ABDGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 게임 인스턴스 가져오기
    GameInstance = Cast<UBDGameInstance>(GetGameInstance());

    // 디버깅용 로그
    UE_LOG(LogTemp, Display, TEXT("BDGameMode: BeginPlay called"));
}

void ABDGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (NewPlayer)
    {
        // 플레이어 시작 위치 찾기
        AActor* StartSpot = FindPlayerStart(NewPlayer);

        if (StartSpot)
        {
            // 선택한 클래스에 맞는 캐릭터 스폰
            APawn* SpawnedPawn = SpawnCharacterForSelectedClass(NewPlayer, StartSpot);

            if (SpawnedPawn)
            {
                // 플레이어 컨트롤러에 폰 할당
                NewPlayer->Possess(SpawnedPawn);
                UE_LOG(LogTemp, Display, TEXT("Character spawned and possessed successfully"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn character"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to find player start"));
        }
    }
}

AActor* ABDGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
    // 기본 플레이어 시작 포인트 찾기
    AActor* FoundPlayerStart = Super::FindPlayerStart_Implementation(Player, IncomingName);

    if (FoundPlayerStart)
    {
        return FoundPlayerStart;
    }

    // 플레이어 시작 포인트가 없으면 레벨에서 PlayerStart 액터 탐색
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* PlayerStart = *It;
        if (PlayerStart)
        {
            return PlayerStart;
        }
    }

    // 찾지 못한 경우 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found in level. Using default spawn location."));
    return nullptr;
}

APawn* ABDGameMode::SpawnCharacterForSelectedClass(APlayerController* Controller, AActor* StartSpot)
{
    if (!Controller || !StartSpot)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid controller or start spot"));
        return nullptr;
    }

    // 게임 인스턴스에서 선택된 클래스 정보 가져오기
    EPlayerClass SelectedClass = GetSelectedPlayerClass();

    // 디버깅용 로그
    FString ClassName;
    switch (SelectedClass)
    {
    case EPlayerClass::Fletcher:
        ClassName = TEXT("Fletcher");
        break;
    case EPlayerClass::Titan:
        ClassName = TEXT("Titan");
        break;
    case EPlayerClass::Igneous:
        ClassName = TEXT("Igneous");
        break;
    default:
        ClassName = TEXT("None");
        break;
    }
    UE_LOG(LogTemp, Display, TEXT("Spawning character for class: %s"), *ClassName);

    // 선택된 클래스에 맞는 캐릭터 클래스 선택
    TSubclassOf<APawn> SelectedCharacterClass = nullptr;

    switch (SelectedClass)
    {
    case EPlayerClass::Fletcher:
        SelectedCharacterClass = BP_Fletcher;
        break;
    case EPlayerClass::Titan:
        SelectedCharacterClass = BP_Titan;
        break;
    case EPlayerClass::Igneous:
        SelectedCharacterClass = BP_Igneous;
        break;
    default:
        UE_LOG(LogTemp, Error, TEXT("No valid class selected. Using default if available."));
        // 기본 값으로 첫 번째 유효한 클래스 사용
        if (BP_Fletcher) SelectedCharacterClass = BP_Fletcher;
        else if (BP_Titan) SelectedCharacterClass = BP_Titan;
        else if (BP_Igneous) SelectedCharacterClass = BP_Igneous;
        break;
    }

    if (!SelectedCharacterClass)
    {
        UE_LOG(LogTemp, Error, TEXT("No valid character class available"));
        return nullptr;
    }

    // 시작 위치와 회전 가져오기
    FVector SpawnLocation = StartSpot->GetActorLocation();
    FRotator SpawnRotation = StartSpot->GetActorRotation();

    // 캐릭터 스폰
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(SelectedCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedPawn)
    {
        UE_LOG(LogTemp, Display, TEXT("Character spawned successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn character"));
    }

    return SpawnedPawn;
}

EPlayerClass ABDGameMode::GetSelectedPlayerClass()
{
    if (GameInstance)
    {
        return GameInstance->GetSelectedClass();
    }

    // 게임 인스턴스가 없거나 선택된 클래스 정보가 없을 경우
    // 게임 인스턴스를 다시 가져와 시도
    GameInstance = Cast<UBDGameInstance>(GetGameInstance());

    if (GameInstance)
    {
        return GameInstance->GetSelectedClass();
    }

    // 여전히 실패한 경우 기본값 반환
    UE_LOG(LogTemp, Error, TEXT("Could not get GameInstance. Using default class."));
    return EPlayerClass::Fletcher; // 기본 클래스 반환 또는 None 반환 가능
}