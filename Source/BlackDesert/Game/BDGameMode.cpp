// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BDGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"

ABDGameMode::ABDGameMode()
{
    // �⺻ �� Ŭ������ ������� ���� (������ Ŭ������ ���� ����)
    DefaultPawnClass = nullptr;
}

void ABDGameMode::BeginPlay()
{
    Super::BeginPlay();

    // ���� �ν��Ͻ� ��������
    GameInstance = Cast<UBDGameInstance>(GetGameInstance());

    // ������ �α�
    UE_LOG(LogTemp, Display, TEXT("BDGameMode: BeginPlay called"));
}

void ABDGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (NewPlayer)
    {
        // �÷��̾� ���� ��ġ ã��
        AActor* StartSpot = FindPlayerStart(NewPlayer);

        if (StartSpot)
        {
            // ������ Ŭ������ �´� ĳ���� ����
            APawn* SpawnedPawn = SpawnCharacterForSelectedClass(NewPlayer, StartSpot);

            if (SpawnedPawn)
            {
                // �÷��̾� ��Ʈ�ѷ��� �� �Ҵ�
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
    // �⺻ �÷��̾� ���� ����Ʈ ã��
    AActor* FoundPlayerStart = Super::FindPlayerStart_Implementation(Player, IncomingName);

    if (FoundPlayerStart)
    {
        return FoundPlayerStart;
    }

    // �÷��̾� ���� ����Ʈ�� ������ �������� PlayerStart ���� Ž��
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* PlayerStart = *It;
        if (PlayerStart)
        {
            return PlayerStart;
        }
    }

    // ã�� ���� ��� �α� ���
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

    // ���� �ν��Ͻ����� ���õ� Ŭ���� ���� ��������
    EPlayerClass SelectedClass = GetSelectedPlayerClass();

    // ������ �α�
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

    // ���õ� Ŭ������ �´� ĳ���� Ŭ���� ����
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
        // �⺻ ������ ù ��° ��ȿ�� Ŭ���� ���
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

    // ���� ��ġ�� ȸ�� ��������
    FVector SpawnLocation = StartSpot->GetActorLocation();
    FRotator SpawnRotation = StartSpot->GetActorRotation();

    // ĳ���� ����
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

    // ���� �ν��Ͻ��� ���ų� ���õ� Ŭ���� ������ ���� ���
    // ���� �ν��Ͻ��� �ٽ� ������ �õ�
    GameInstance = Cast<UBDGameInstance>(GetGameInstance());

    if (GameInstance)
    {
        return GameInstance->GetSelectedClass();
    }

    // ������ ������ ��� �⺻�� ��ȯ
    UE_LOG(LogTemp, Error, TEXT("Could not get GameInstance. Using default class."));
    return EPlayerClass::Fletcher; // �⺻ Ŭ���� ��ȯ �Ǵ� None ��ȯ ����
}