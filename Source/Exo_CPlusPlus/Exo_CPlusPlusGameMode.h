// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Exo_CPlusPlusGameMode.generated.h"

UCLASS(minimalapi)
class AExo_CPlusPlusGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AExo_CPlusPlusGameMode();
	void Respawn(AController* controller);

protected:
	UFUNCTION()
	void Spawn(AController* controller);
	FTimerHandle RespawnHandle;
};



