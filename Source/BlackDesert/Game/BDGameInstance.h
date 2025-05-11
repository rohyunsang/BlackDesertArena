// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BDGameInstance.generated.h"

UENUM(BlueprintType)
enum class EPlayerClass : uint8
{
	None UMETA(DisplayName = "None"),
	Fletcher UMETA(DisplayName = "Fletcher"),
	Titan UMETA(DisplayName = "Titan"),
	Igneous UMETA(DisplayName = "Igneous")
};

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UBDGameInstance();

    // ���õ� Ŭ���� ����
    UPROPERTY(BlueprintReadWrite, Category = "Game")
    EPlayerClass SelectedPlayerClass;

    // Ŭ���� ���� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetSelectedClass(EPlayerClass NewClass);

    // Ŭ���� �������� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Game")
    EPlayerClass GetSelectedClass() const { return SelectedPlayerClass; }

    // ���� ������ �̵��ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Game")
    void LoadGameLevel(FString LevelName);

public:
	virtual void Init() override;

private:
	void TryLoginWithDeviceId();
	
};
