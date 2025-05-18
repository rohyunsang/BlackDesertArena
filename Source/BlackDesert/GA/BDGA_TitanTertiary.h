// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/BDGA_BasicAttack.h"
#include "BDGA_TitanTertiary.generated.h"

class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class BLACKDESERT_API UBDGA_TitanTertiary : public UBDGA_BasicAttack
{
	GENERATED_BODY()
	
public:
	UBDGA_TitanTertiary();

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    UAnimMontage* SecondAttackMontage;

    // ĳ���� �� ����� ����Ʈ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Effects")
    UNiagaraSystem* CastingEffect;

    // ��ų ������ ����� ����Ʈ 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Effects")
    UNiagaraSystem* SkillEffect;

    // ��ų ������ ����� ����Ʈ 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Effects")
    UNiagaraSystem* SkillEffect2;

    // �浹 �� ����� ����Ʈ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Effects")
    UNiagaraSystem* HitEffect;

    // ���� ���� (�������� �󸶳� �ָ� �����ϴ���)
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Range")
    float AttackRange = 150.0f;

    // �� ��° ���� ���� �Լ�
    UFUNCTION()
    void PerformSecondAttack();

    int32 CurrentAttackCount = 0;

private:
    TSet<AActor*> AlreadyHitActors;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Movement")
    float DashDistance = 200.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Attack|Movement")
    bool bPreventFalling = true;

    UFUNCTION()
    void PerformDash(ACharacter* Character);
};
