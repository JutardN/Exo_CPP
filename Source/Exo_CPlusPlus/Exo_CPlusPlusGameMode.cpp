// Copyright Epic Games, Inc. All Rights Reserved.

#include "Exo_CPlusPlusGameMode.h"
#include "Exo_CPlusPlusCharacter.h"
#include "UObject/ConstructorHelpers.h"

AExo_CPlusPlusGameMode::AExo_CPlusPlusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
