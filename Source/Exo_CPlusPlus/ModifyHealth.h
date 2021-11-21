// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Engine.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Exo_CPlusPlusCharacter.h"
#include "GameFramework/Character.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModifyHealth.generated.h"


UCLASS()
class EXO_CPLUSPLUS_API AModifyHealth : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AModifyHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	int amount;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxTrigger;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* Light;

	FTimerHandle ModifyHandle;

	UFUNCTION()
	void ModifyOverTime(AExo_CPlusPlusCharacter* player);

private:
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
