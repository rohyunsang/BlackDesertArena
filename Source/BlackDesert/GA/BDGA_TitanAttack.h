// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/BDGA_BasicAttack.h"
#include "BDGA_TitanAttack.generated.h"

// ���̾ư��� ����� ���� ����
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDGA_TitanAttack : public UBDGA_BasicAttack
{
	GENERATED_BODY()
	

public:
    UBDGA_TitanAttack();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    // ��Ƽ���� �ݹ� �Լ� - ���� ���� ����
    // UAnimNotify �̺�Ʈ �ݹ� (OnPlayMontageNotifyBegin ��������Ʈ��)
    UFUNCTION()
    void OnAttackNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

    // ���� ���� ���� �Լ�
    UFUNCTION()
    void ExecuteAttack();

    // ��Ÿ�� ���� �ݹ� �Լ� (OnMontageEnded ��������Ʈ��)
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // ���� �浹 �ڽ� ũ��
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Collision")
    FVector AttackBoxExtent = FVector(500.0f, 300.0f, 100.0f);

    // ����� �ð�ȭ ����
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Debug")
    bool bShowDebugTrace = true;

    // ���� �����
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Damage")
    float BaseDamage = 20.0f;


    // �浹 �� ����� ����Ʈ
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Effects")
    UNiagaraSystem* HitEffect;

    // ���� ���� (�������� �󸶳� �ָ� �����ϴ���)
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Range")
    float AttackRange = 150.0f;

protected:
    // HitEffect�� ���� �����ϱ� ���� �Լ�
    UFUNCTION()
    void PlayDelayedHitEffect(FVector Location, FRotator Rotation);

private:
    TSet<AActor*> AlreadyHitActors;

};
