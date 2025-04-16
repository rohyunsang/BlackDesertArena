// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BDGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"


void UBDGameInstance::Init()
{
	Super::Init();
	// TryLoginWithDeviceId(); Be Later 
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
