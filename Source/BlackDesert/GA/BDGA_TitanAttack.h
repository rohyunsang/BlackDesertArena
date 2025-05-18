// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/BDGA_BasicAttack.h"
#include "BDGA_TitanAttack.generated.h"

// 나이아가라 사용을 위한 선언
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

    // 노티파이 콜백 함수 - 공격 판정 수행
    // UAnimNotify 이벤트 콜백 (OnPlayMontageNotifyBegin 델리게이트용)
    UFUNCTION()
    void OnAttackNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

    // 공격 판정 수행 함수
    UFUNCTION()
    void ExecuteAttack();

    // 몽타주 종료 콜백 함수 (OnMontageEnded 델리게이트용)
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 공격 충돌 박스 크기
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Collision")
    FVector AttackBoxExtent = FVector(500.0f, 300.0f, 100.0f);

    // 디버그 시각화 여부
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Debug")
    bool bShowDebugTrace = true;

    // 공격 대미지
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Damage")
    float BaseDamage = 20.0f;


    // 충돌 시 재생할 이펙트
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Effects")
    UNiagaraSystem* HitEffect;

    // 공격 범위 (전방으로 얼마나 멀리 공격하는지)
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Range")
    float AttackRange = 150.0f;

protected:
    // HitEffect를 지연 실행하기 위한 함수
    UFUNCTION()
    void PlayDelayedHitEffect(FVector Location, FRotator Rotation);

private:
    TSet<AActor*> AlreadyHitActors;

};
