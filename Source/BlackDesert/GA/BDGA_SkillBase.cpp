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

    // ��Ÿ�� ����
    if (CooldownEffect)
    {
        ApplyCooldown(Handle, ActorInfo, ActivationInfo);
        UE_LOG(LogTemp, Log, TEXT("BD_LOG CooldownEffect Applied: %s"), *CooldownEffect->GetName());
    }

    // �ڽ� Ŭ�������� ��ų ���� ���� ����
    // ��: ����Ʈ, ����, ����ü ��

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
