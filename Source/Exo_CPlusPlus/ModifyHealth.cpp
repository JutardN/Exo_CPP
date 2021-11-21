// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifyHealth.h"
#include "TimerManager.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AModifyHealth::AModifyHealth()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxTrigger->SetSimulatePhysics(false);
	BoxTrigger->SetMobility(EComponentMobility::Static);
	BoxTrigger->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = BoxTrigger;

	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AModifyHealth::OnOverlapBegin);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);

	Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire"));
	Fire->SetupAttachment(RootComponent);
	Fire->SetWorldRotation(FRotator(0,90,0));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>Particle(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
	fireParticle = Particle.Object;
}

// Called when the game starts or when spawned
void AModifyHealth::BeginPlay()
{
	Super::BeginPlay();
	if (amount > 0) {
		Light->SetLightColor(FColor::Green);
	}
	else {
		Light->SetLightColor(FColor::Red);
		Fire->SetTemplate(fireParticle);
	}

	BoxTrigger->SetGenerateOverlapEvents(true);
	BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &AModifyHealth::OnOverlapEnd);
}

// Called every frame
void AModifyHealth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AModifyHealth::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AExo_CPlusPlusCharacter* player = Cast<AExo_CPlusPlusCharacter>(OtherActor);
	if (!player)
		return;
	
	FTimerDelegate ModifyDele;
	ModifyDele.BindUFunction(this, FName("ModifyOverTime"), player);
	if (amount > 0) {
		GetWorld()->GetTimerManager().SetTimer(ModifyHandle, ModifyDele, 1, true, 0);
	}
	else {
		GetWorld()->GetTimerManager().SetTimer(ModifyHandle, ModifyDele, 0.5f, true,0);
	}

}

void AModifyHealth::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorld()->GetTimerManager().ClearTimer(ModifyHandle);
}

void AModifyHealth::ModifyOverTime(AExo_CPlusPlusCharacter* player)
{
	player->health += amount;
	if (player->health <= 0) {
		player->health = 0;
		GetWorld()->GetTimerManager().ClearTimer(ModifyHandle);
		player->Death();
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player health : %d"), player->health));
}