// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BDGameInstance.h"
#include "BDGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDESERT_API ABDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    ABDGameMode();

    // 게임 시작시 호출
    virtual void BeginPlay() override;

    // 플레이어 컨트롤러가 로그인할 때 호출
    virtual void PostLogin(APlayerController* NewPlayer) override;

    // 플레이어가 스폰될 위치 찾기
    virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

protected:
    // 각 캐릭터 클래스 블루프린트 레퍼런스
    UPROPERTY(EditDefaultsOnly, Category = "Characters")
    TSubclassOf<APawn> BP_Fletcher;

    UPROPERTY(EditDefaultsOnly, Category = "Characters")
    TSubclassOf<APawn> BP_Titan;

    UPROPERTY(EditDefaultsOnly, Category = "Characters")
    TSubclassOf<APawn> BP_Igneous;

    // 선택된 클래스에 따라 캐릭터 스폰
    UFUNCTION()
    APawn* SpawnCharacterForSelectedClass(APlayerController* Controller, AActor* StartSpot);

    // 게임 인스턴스에서 선택된 클래스 정보 가져오기
    UFUNCTION()
    EPlayerClass GetSelectedPlayerClass();

private:
    // 게임 인스턴스 캐시
    UPROPERTY()
    UBDGameInstance* GameInstance;
	
};
