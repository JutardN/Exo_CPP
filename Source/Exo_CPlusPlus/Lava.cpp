// Fill out your copyright notice in the Description page of Project Settings.

#include "Exo_CPlusPlusCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Lava.h"

// Sets default values
ALava::ALava()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetMobility(EComponentMobility::Static);
	Mesh->SetCollisionProfileName(TEXT("Trigger"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
	UStaticMesh* Cube = MeshAsset.Object;
	Mesh->SetStaticMesh(Cube);

	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/MyModif/Lava.Lava'"));
	UMaterial* mat = Material.Object;
	Mesh->SetMaterial(0, mat);

	RootComponent = Mesh;
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ALava::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ALava::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALava::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALava::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AExo_CPlusPlusCharacter* player = Cast<AExo_CPlusPlusCharacter>(OtherActor);
	if (!player)
		return;

	player->Death();
}