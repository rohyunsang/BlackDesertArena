// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/BDPerformHitNotify.h"
#include "Monster/Component/BDMonsterFSMComponent.h"
#include "GameFramework/Character.h"

void UBDPerformHitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACharacter* Owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Owner) return;

	UBDMonsterFSMComponent* FSM = Owner->FindComponentByClass<UBDMonsterFSMComponent>();
	if (FSM)
	{
		FSM->PerformAttackHitCheck(); // 데미지 판정 수행
	}
}