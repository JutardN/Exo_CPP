// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnerItems.h"

// Sets default values
ASpawnerItems::ASpawnerItems()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnerItems::BeginPlay()
{
	Super::BeginPlay();
	int32 rand = FMath::RandRange(0, TabItems->GetRowNames().Num()-1);
	//SaveItems = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	//SaveItems->OnComponentHit.AddDynamic(this, &ASpawnerItems::OnHit);
}

// Called every frame
void ASpawnerItems::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}