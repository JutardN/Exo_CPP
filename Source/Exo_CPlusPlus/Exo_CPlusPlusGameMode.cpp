// Copyright Epic Games, Inc. All Rights Reserved.

#include "Exo_CPlusPlusGameMode.h"
#include "Exo_CPlusPlusCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

AExo_CPlusPlusGameMode::AExo_CPlusPlusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AExo_CPlusPlusGameMode::Spawn(AController* controller)
{
	if (controller)
	{
		FVector Location = FVector(-970.0f, -346.0f, 202.0f);
		FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f);
		if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, Rotation)) {
			controller->Possess(Pawn);
		}
	}
}

void AExo_CPlusPlusGameMode::Respawn(AController* controller) 
{
	FTimerDelegate RespawnDele;
	RespawnDele.BindUFunction(this, FName("Spawn"), controller);
	GetWorld()->GetTimerManager().SetTimer(RespawnHandle, RespawnDele, 3.0f, false);
}