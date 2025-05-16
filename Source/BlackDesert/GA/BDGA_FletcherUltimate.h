// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/BDGA_FletcherTertiary.h"
#include "BDGA_FletcherUltimate.generated.h"

class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDGA_FletcherUltimate : public UBDGA_FletcherTertiary
{
	GENERATED_BODY()
	

protected:
    // PerformDash �Լ� �������̵�
    virtual void PerformDash(ACharacter* Character) override;

    // �ʿ��� �߰� ������Ƽ (��: �ٸ� ��� �Ÿ�, ���� ��)
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Movement")
    float UltimateDashDistance = 300.0f; // �� �� ��� �Ÿ� ����


    UPROPERTY(EditDefaultsOnly, Category = "Attack|Movement")
    float JumpHeight = 500.0f; // ������ ���̷� ����

protected:
    FTimerHandle JumpTimerHandle;
    UFUNCTION()
    void AdjustFallVelocity(ACharacter* Character);

    // �ְ� ���̿� �����ϴ� �ð�(��)
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Movement")
    float TimeToApex = 0.5f;
};
