// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/PawnSensingComponent.h"
#include "BDPawnSensingComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = "Monster", meta = (BlueprintSpawnableComponent))
class BLACKDESERT_API UBDPawnSensingComponent : public UPawnSensingComponent
{
	GENERATED_BODY()

public:
	UBDPawnSensingComponent();

	
	/** ����: See/Hear ��������Ʈ�� �� ���� ���ε� */
	template<typename UserClass>
	void BindCallbacks(
		UserClass* Object,
		void (UserClass::* SeeFunc)(APawn*),                                        // OnSeePawn
		void (UserClass::* HearFunc)(APawn*, const FVector&, float) = nullptr)      // OnHearNoise
	{
		if (SeeFunc)  OnSeePawn.AddDynamic(Object, SeeFunc);
		if (HearFunc) OnHearNoise.AddDynamic(Object, HearFunc);
	}
	
};
