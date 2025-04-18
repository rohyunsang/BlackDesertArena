// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BDPawnSensingComponent.h"

UBDPawnSensingComponent::UBDPawnSensingComponent()
{
	// 기본값을 프로젝트에 맞게 조정
	SightRadius = 1200.f;
	SetPeripheralVisionAngle(60.f);

	HearingThreshold = 800.f;
	LOSHearingThreshold = 1000.f;
	bHearNoises = true;

	// 업데이트 주기 (0.25s = 4회/초)
	SensingInterval = 0.25f;
}