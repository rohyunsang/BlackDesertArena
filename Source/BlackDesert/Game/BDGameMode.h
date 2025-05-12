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

    // ���� ���۽� ȣ��
    virtual void BeginPlay() override;

    // �÷��̾� ��Ʈ�ѷ��� �α����� �� ȣ��
    virtual void PostLogin(APlayerController* NewPlayer) override;

    // �÷��̾ ������ ��ġ ã��
    virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

protected:
    // �� ĳ���� Ŭ���� �������Ʈ ���۷���
    UPROPERTY(EditDefaultsOnly, Category = "Characters")
    TSubclassOf<APawn> BP_Fletcher;

    UPROPERTY(EditDefaultsOnly, Category = "Characters")
    TSubclassOf<APawn> BP_Titan;

    UPROPERTY(EditDefaultsOnly, Category = "Characters")
    TSubclassOf<APawn> BP_Igneous;

    // ���õ� Ŭ������ ���� ĳ���� ����
    UFUNCTION()
    APawn* SpawnCharacterForSelectedClass(APlayerController* Controller, AActor* StartSpot);

    // ���� �ν��Ͻ����� ���õ� Ŭ���� ���� ��������
    UFUNCTION()
    EPlayerClass GetSelectedPlayerClass();

private:
    // ���� �ν��Ͻ� ĳ��
    UPROPERTY()
    UBDGameInstance* GameInstance;
	
};
