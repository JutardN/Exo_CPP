// Fill out your copyright notice in the Description page of Project Settings.


#include "Bumper.h"

// Sets default values
ABumper::ABumper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetSimulatePhysics(true);
	RootComponent = StaticMesh;
	StaticMesh->OnComponentHit.AddDynamic(this, &ABumper::OnHit);
}

// Called when the game starts or when spawned
void ABumper::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABumper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABumper::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* player = Cast<ACharacter>(OtherActor);
	if (!player)
		return;

	player->LaunchCharacter(FVector(0.0f, 0.0f, powerPush), false, false);
}