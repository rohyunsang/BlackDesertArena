// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/BDGA_FletcherUltimate.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBDGA_FletcherUltimate::PerformDash(ACharacter* Character)
{
    if (!Character) return;

    // ĳ���Ͱ� �ٶ󺸴� ���� ��������
    FVector ForwardVector = Character->GetActorForwardVector();

    float offset = 8.f;

    // �̵��� ���� ��� - �ڷ� �̵�
    FVector DashVector = -ForwardVector * UltimateDashDistance * offset;

    // ���� ���� �߰� (Z���� ����� ����)
    DashVector.Z = JumpHeight;

    // ĳ���� �߻�(�뽬)
    Character->LaunchCharacter(DashVector, true, true);

    UE_LOG(LogTemp, Log, TEXT("Ultimate Dash performed: %s"), *DashVector.ToString());

    // �ְ� ���̿� ������ �ð��� ���� Ÿ�̸� ����
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

    // ĳ������ ���� �ӵ� ��������
    FVector CurrentVelocity = Character->GetVelocity();

    // ���� �ӵ�(X, Y)�� 0���� �����ϰ� Z �ӵ��� ���� (�Ǵ� ���ϴ� ���� �ӵ��� ����)
    FVector NewVelocity = FVector(0.0f, 0.0f, CurrentVelocity.Z);

    // �߷��� �ۿ��ϹǷ� Z �ӵ��� �״�� �ΰų�, 
    // �� ���� ���ϸ� ���Ѵٸ� �� ū ���� ������ ������ �� ����
    NewVelocity.Z = -500.0f; // ���� ���ϸ� ���Ѵٸ� �̷��� ����

    // ĳ������ �ӵ� ������Ʈ
    Character->GetCharacterMovement()->Velocity = NewVelocity;

    UE_LOG(LogTemp, Log, TEXT("Fall velocity adjusted: %s"), *NewVelocity.ToString());
}