// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlackDesertGameMode.h"
#include "BlackDesertCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABlackDesertGameMode::ABlackDesertGameMode()
{
	// /Script/Engine.Blueprint'/Game/Blueprint/Character/BP_BDCharacter.BP_BDCharacter'
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Character/BP_BDArcher"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
