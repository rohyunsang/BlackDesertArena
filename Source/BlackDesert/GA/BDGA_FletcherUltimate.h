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
    // PerformDash 함수 오버라이드
    virtual void PerformDash(ACharacter* Character) override;

    // 필요한 추가 프로퍼티 (예: 다른 대시 거리, 방향 등)
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Movement")
    float UltimateDashDistance = 300.0f; // 더 긴 대시 거리 예시


    UPROPERTY(EditDefaultsOnly, Category = "Attack|Movement")
    float JumpHeight = 500.0f; // 적절한 높이로 조정

protected:
    FTimerHandle JumpTimerHandle;
    UFUNCTION()
    void AdjustFallVelocity(ACharacter* Character);

    // 최고 높이에 도달하는 시간(초)
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Movement")
    float TimeToApex = 0.5f;
};
