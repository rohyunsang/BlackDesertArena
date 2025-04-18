// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BDPawnSensingComponent.h"

UBDPawnSensingComponent::UBDPawnSensingComponent()
{
	// �⺻���� ������Ʈ�� �°� ����
	SightRadius = 1200.f;
	SetPeripheralVisionAngle(60.f);

	HearingThreshold = 800.f;
	LOSHearingThreshold = 1000.f;
	bHearNoises = true;

	// ������Ʈ �ֱ� (0.25s = 4ȸ/��)
	SensingInterval = 0.25f;
}