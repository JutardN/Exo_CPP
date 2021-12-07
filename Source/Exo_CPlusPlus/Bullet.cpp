// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet.h"

#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Exo_CPlusPlusCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetMobility(EComponentMobility::Movable);
	//Mesh->SetCollisionProfileName(TEXT("Trigger"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	UStaticMesh* Cube = MeshAsset.Object;
	Mesh->SetStaticMesh(Cube);

	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'"));
	UMaterial* mat = Material.Object;
	Mesh->SetMaterial(0, mat);

	Mesh->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));

	projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	projectileMovement->ProjectileGravityScale = 2;
	projectileMovement->InitialSpeed = 5000;

	RootComponent = Mesh;
	//colliderSphere->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	Mesh->OnComponentHit.AddDynamic(this, &ABullet::OnHit);

	static ConstructorHelpers::FObjectFinder<UMaterial>Decal(TEXT("Material'/Game/MyModif/Logo.Logo'"));
	DecalToSpawn = Decal.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem>Particle(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	ParticleToSpawn = Particle.Object;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ABullet::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	AExo_CPlusPlusCharacter* player = Cast<AExo_CPlusPlusCharacter>(OtherActor);
//	if (player)
//		return;
//
//	ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(SweepResult.Location, FRotator());
//	Destroy();
//}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AExo_CPlusPlusCharacter* player = Cast<AExo_CPlusPlusCharacter>(OtherActor);
	if (player) {
		//Destroy();
		return;
	}
	
	ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(Hit.Location, Hit.ImpactNormal.Rotation());
	decal->SetActorRelativeRotation(Hit.ImpactNormal.Rotation());

	if (decal)
	{
		decal->SetDecalMaterial(DecalToSpawn);
		decal->SetLifeSpan(2.0f);
		decal->GetDecal()->DecalSize = FVector(30, 30, 30);
	}
	decal->AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepWorldTransform);

	UGameplayStatics::SpawnEmitterAtLocation(Hit.GetComponent(), ParticleToSpawn, Hit.Location, Hit.ImpactNormal.Rotation(), true);
	
	Destroy();
}