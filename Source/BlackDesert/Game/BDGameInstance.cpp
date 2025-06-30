// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BDGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"
#include "OnlineSessionSettings.h"

UBDGameInstance::UBDGameInstance()
{
    SelectedPlayerClass = EPlayerClass::None;
}

void UBDGameInstance::Init()
{
	Super::Init();

    SelectedPlayerClass = EPlayerClass::None;

    TryLoginWithDeviceId(); 

    // 해상도 강제 설정
    if (GEngine && GEngine->GameUserSettings)
    {
        GEngine->GameUserSettings->SetScreenResolution(FIntPoint(1250, 720));
        GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::Windowed); // 또는 Fullscreen, Windowed
        GEngine->GameUserSettings->ApplySettings(false); // true이면 디스크에 저장
    }
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
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem is null "));
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("IdentityInterface is null "));
        return;
    }

    // Device ID 생성
    FString DeviceId = FPlatformMisc::GetDeviceId();
    UE_LOG(LogTemp, Warning, TEXT("Device ID: %s"), *DeviceId);

    FOnlineAccountCredentials Credentials;
    Credentials.Type = TEXT("deviceid");
    Credentials.Id = DeviceId;
    Credentials.Token = TEXT("");

    // 델리게이트를 멤버 변수로 저장 (람다가 소멸되지 않도록)
    Identity->OnLoginCompleteDelegates->AddUObject(this, &UBDGameInstance::OnDeviceIdLoginComplete);

    bool bLoginStarted = Identity->Login(0, Credentials);
    UE_LOG(LogTemp, Warning, TEXT("login : %s"), bLoginStarted ? TEXT("success") : TEXT("fail"));
}

void UBDGameInstance::OnDeviceIdLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Device ID login success: %s"), *UserId.ToString());

        // 로그인 성공 후에 세션 생성
        CreateSession();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Device ID login fail : %s"), *Error);

        // 실패 시 대안: LAN 세션으로 진행
        // CreateLANSession();
    }

    // 델리게이트 해제
    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
        {
            Identity->OnLoginCompleteDelegates->RemoveAll(this);
        }
    }
}

void UBDGameInstance::CreateSession() 
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
    if (!Sessions.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("SessionInterface not available"));
        return;
    }

    // 기존 세션이 있다면 제거
    FNamedOnlineSession* ExistingSession = Sessions->GetNamedSession(NAME_GameSession);
    if (ExistingSession)
    {
        Sessions->DestroySession(NAME_GameSession);
    }

    // 세션 설정 생성
    TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

    SessionSettings->bIsLANMatch = false;
    SessionSettings->NumPublicConnections = 2;
    SessionSettings->NumPrivateConnections = 0;
    SessionSettings->bAllowInvites = true;
    SessionSettings->bAllowJoinInProgress = true;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->bUsesPresence = true; // LAN이 아닐 때만 Presence 사용
    SessionSettings->bUseLobbiesIfAvailable = true;
    SessionSettings->bAllowJoinViaPresence = true;
    SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
    SessionSettings->bAntiCheatProtected = false;

    // 커스텀 세션 설정 추가
    // SessionSettings->Set(FName("BattleMap"), FString("Test"), EOnlineDataAdvertisementType::ViaOnlineService);
    // SessionSettings->Set(SETTING_GAMEMODE, FString("BDGameMode"), EOnlineDataAdvertisementType::ViaOnlineService);

    // 선택된 플레이어 클래스 정보도 세션에 저장
    FString SelectedClassName;
    switch (SelectedPlayerClass)
    {
    case EPlayerClass::Fletcher:
        SelectedClassName = TEXT("Fletcher");
        break;
    case EPlayerClass::Titan:
        SelectedClassName = TEXT("Titan");
        break;
    case EPlayerClass::Igneous:
        SelectedClassName = TEXT("Igneous");
        break;
    default:
        SelectedClassName = TEXT("None");
        break;
    }
    //SessionSettings->Set(FName("SelectedClass"), SelectedClassName, EOnlineDataAdvertisementType::ViaOnlineService);

    // 세션 생성 완료 델리게이트 바인딩
    Sessions->OnCreateSessionCompleteDelegates.AddUObject(this, &UBDGameInstance::OnCreateSessionComplete);

    // 세션 생성
    bool bCreateResult = Sessions->CreateSession(0, FName("RohSession"), *SessionSettings);

    if (!bCreateResult)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to start session creation"));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create session"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Session creation started..."));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Creating session..."));
        }
    }
}

void UBDGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session created successfully: %s"), *SessionName.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
                FString::Printf(TEXT("Session '%s' created successfully!"), *SessionName.ToString()));
        }

        // /Script/Engine.World'/Game/Map/BattleMap.BattleMap'
        // 세션 생성 성공 후 게임 레벨로 이동하고 싶다면 여기서 호출
        LoadGameLevel(TEXT("BattleMap"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session: %s"), *SessionName.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("Failed to create session '%s'"), *SessionName.ToString()));
        }
    }
}