// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/BDGA_SkillBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"

UBDGA_SkillBase::UBDGA_SkillBase()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBDGA_SkillBase::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    UE_LOG(LogTemp, Warning, TEXT("BD_LOG SkillBase::ActivateAbility() called"));

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // 쿨타임 적용
    if (CooldownEffect)
    {
        ApplyCooldown(Handle, ActorInfo, ActivationInfo);
        UE_LOG(LogTemp, Log, TEXT("BD_LOG CooldownEffect Applied: %s"), *CooldownEffect->GetName());
    }

    // 자식 클래스에서 스킬 개별 동작 실행
    // 예: 이펙트, 사운드, 투사체 등

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
