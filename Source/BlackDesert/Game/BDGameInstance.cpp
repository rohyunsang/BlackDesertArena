// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BDGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"


UBDGameInstance::UBDGameInstance()
{
    SelectedPlayerClass = EPlayerClass::None;
}

void UBDGameInstance::Init()
{
	Super::Init();

    SelectedPlayerClass = EPlayerClass::None;

    // TryLoginWithDeviceId(); Be Later 
}


void UBDGameInstance::SetSelectedClass(EPlayerClass NewClass)
{
    SelectedPlayerClass = NewClass;

    // 로그로 확인
    if (GEngine)
    {
        FString ClassName;
        switch (SelectedPlayerClass)
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
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
            FString::Printf(TEXT("Selected Class: %s"), *ClassName));
    }
}

void UBDGameInstance::LoadGameLevel(FString LevelName)
{
    // 클래스가 선택되었는지 확인
    if (SelectedPlayerClass == EPlayerClass::None)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                TEXT("Please select a class before starting the game!"));
        }
        return;
    }

    // 게임 레벨로 이동 (Level 이름을 실제 프로젝트에 맞게 수정하세요)
    UGameplayStatics::OpenLevel(this, FName(LevelName));

    // 또는 특정 레벨로 이동하려면:
    // UGameplayStatics::OpenLevel(this, TEXT("YourLevelName"));
}


void UBDGameInstance::TryLoginWithDeviceId()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity.IsValid()) return;

    FString DeviceId = FPlatformMisc::GetDeviceId();
    FOnlineAccountCredentials Credentials;
    Credentials.Type = TEXT("deviceid");
    Credentials.Id = DeviceId;
    Credentials.Token = TEXT("");

    Identity->OnLoginCompleteDelegates->AddLambda(
        [](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error) {
            if (bWasSuccessful)
            {
                UE_LOG(LogTemp, Log, TEXT("? Device ID Login Success: %s"), *UserId.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("? Device ID Login Fail: %s"), *Error);
            }
        });

    Identity->Login(0, Credentials);
}
