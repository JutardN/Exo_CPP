// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadLevel.h"

// Sets default values
ALoadLevel::ALoadLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxTrigger->SetSimulatePhysics(false);
	BoxTrigger->SetMobility(EComponentMobility::Static);
	BoxTrigger->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = BoxTrigger;
	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALoadLevel::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ALoadLevel::BeginPlay()
{
	Super::BeginPlay();
	BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &ALoadLevel::OnOverlapEnd);
}

// Called every frame
void ALoadLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALoadLevel::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AExo_CPlusPlusCharacter* player = Cast<AExo_CPlusPlusCharacter>(OtherActor);
	if (!player)
		return;
	if (LevelToLoad == "")
		return;

	FString text = LevelToLoad.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Load : %s"), *text));

	FLatentActionInfo LatentInfo;
	UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
}

void ALoadLevel::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	AExo_CPlusPlusCharacter* player = Cast<AExo_CPlusPlusCharacter>(OtherActor);
	if (!player)
		return;
	if (LevelToLoad == "")
		return;

	FString text = LevelToLoad.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Unload : %s"), *text));

	FLatentActionInfo LatentInfo;
	UGameplayStatics::UnloadStreamLevel(this, LevelToLoad, LatentInfo,true);
}
