// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5TestTaskGameMode.h"
#include "UE5TestTaskCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE5TestTaskGameMode::AUE5TestTaskGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
