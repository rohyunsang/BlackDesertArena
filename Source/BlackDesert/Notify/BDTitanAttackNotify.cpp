// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/BDTitanAttackNotify.h"
#include "GA/BDGA_TitanAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UBDTitanAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    UE_LOG(LogTemp, Warning, TEXT("BDTitanAttackNotify::Notify call"));
}