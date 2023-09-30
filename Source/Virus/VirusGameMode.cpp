// Copyright Epic Games, Inc. All Rights Reserved.

#include "VirusGameMode.h"
#include "VirusCharacter.h"
#include "UObject/ConstructorHelpers.h"

AVirusGameMode::AVirusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
