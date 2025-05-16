// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_FletcherUltimate.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBDGA_FletcherUltimate::PerformDash(ACharacter* Character)
{
    if (!Character) return;

    // 캐릭터가 바라보는 방향 가져오기
    FVector ForwardVector = Character->GetActorForwardVector();

    float offset = 8.f;

    // 이동할 벡터 계산 - 뒤로 이동
    FVector DashVector = -ForwardVector * UltimateDashDistance * offset;

    // 위로 점프 추가 (Z값을 양수로 설정)
    DashVector.Z = JumpHeight;

    // 캐릭터 발사(대쉬)
    Character->LaunchCharacter(DashVector, true, true);

    UE_LOG(LogTemp, Log, TEXT("Ultimate Dash performed: %s"), *DashVector.ToString());

    // 최고 높이에 도달할 시간에 맞춰 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(
        JumpTimerHandle,
        FTimerDelegate::CreateUObject(this, &UBDGA_FletcherUltimate::AdjustFallVelocity, Character),
        TimeToApex,
        false
    );
}

void UBDGA_FletcherUltimate::AdjustFallVelocity(ACharacter* Character)
{
    if (!Character) return;

    // 캐릭터의 현재 속도 가져오기
    FVector CurrentVelocity = Character->GetVelocity();

    // 수평 속도(X, Y)를 0으로 설정하고 Z 속도만 유지 (또는 원하는 낙하 속도로 설정)
    FVector NewVelocity = FVector(0.0f, 0.0f, CurrentVelocity.Z);

    // 중력이 작용하므로 Z 속도는 그대로 두거나, 
    // 더 빠른 낙하를 원한다면 더 큰 음수 값으로 설정할 수 있음
    NewVelocity.Z = -500.0f; // 빠른 낙하를 원한다면 이렇게 설정

    // 캐릭터의 속도 업데이트
    Character->GetCharacterMovement()->Velocity = NewVelocity;

    UE_LOG(LogTemp, Log, TEXT("Fall velocity adjusted: %s"), *NewVelocity.ToString());
}