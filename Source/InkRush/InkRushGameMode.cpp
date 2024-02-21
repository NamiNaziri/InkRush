// Copyright Epic Games, Inc. All Rights Reserved.

#include "InkRushGameMode.h"
#include "InkRushCharacter.h"
#include "UObject/ConstructorHelpers.h"

AInkRushGameMode::AInkRushGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
