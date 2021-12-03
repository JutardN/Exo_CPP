// Copyright Epic Games, Inc. All Rights Reserved.

#include "Exo_CPlusPlusCharacter.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Exo_CPlusPlusGameMode.h"
#include "DrawDebugHelpers.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Bullet.h"
#include "MySaveGame.h"
#include "MyGameInstance.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AExo_CPlusPlusCharacter

AExo_CPlusPlusCharacter::AExo_CPlusPlusCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

}

//////////////////////////////////////////////////////////////////////////
// Input

void AExo_CPlusPlusCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed,this, &AExo_CPlusPlusCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released,this, &AExo_CPlusPlusCharacter::StopCrouching);
	PlayerInputComponent->BindAction("PickUp", IE_Pressed,this, &AExo_CPlusPlusCharacter::PickUpObject);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed,this, &AExo_CPlusPlusCharacter::Shoot);

	PlayerInputComponent->BindAction("Save", IE_Pressed, this, &AExo_CPlusPlusCharacter::SaveGame);
	PlayerInputComponent->BindAction("Load", IE_Pressed, this, &AExo_CPlusPlusCharacter::LoadGame);

	PlayerInputComponent->BindAxis("MoveForward", this, &AExo_CPlusPlusCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AExo_CPlusPlusCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AExo_CPlusPlusCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AExo_CPlusPlusCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AExo_CPlusPlusCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AExo_CPlusPlusCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AExo_CPlusPlusCharacter::OnResetVR);
	UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI->continueLevel) {
		LoadGame();
	}
}


void AExo_CPlusPlusCharacter::OnResetVR()
{
	// If Exo_CPlusPlus is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in Exo_CPlusPlus.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AExo_CPlusPlusCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
		isCrouching = false;
}

void AExo_CPlusPlusCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AExo_CPlusPlusCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AExo_CPlusPlusCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AExo_CPlusPlusCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AExo_CPlusPlusCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AExo_CPlusPlusCharacter::StartCrouch() 
{
	isCrouching = true;
	Crouch();
}

void AExo_CPlusPlusCharacter::StopCrouching() 
{
	isCrouching = false;
	UnCrouch();
}

void AExo_CPlusPlusCharacter::Death() {

	auto capsule = GetCapsuleComponent();
	auto mesh = GetMesh();

	GetCapsuleComponent()->DestroyComponent();
	this->GetCharacterMovement()->DisableMovement();
	capsule->SetCollisionProfileName("PhysicsActor");
	mesh->SetCollisionProfileName("PhysicsActor");
	mesh->SetSimulatePhysics(true);
	DropObject();
	APlayerController* controller= UGameplayStatics::GetPlayerController(GetWorld(), 0);
	controller->UnPossess();
	
	AGameModeBase* gameMode = GetWorld()->GetAuthGameMode();
	if (AExo_CPlusPlusGameMode* GameMode = Cast<AExo_CPlusPlusGameMode>(gameMode)) {
		GetFollowCamera()->DestroyComponent();
		GetCameraBoom()->DestroyComponent();
		FollowCamera = NULL;
		GameMode->Respawn(controller);
	}
}

void AExo_CPlusPlusCharacter::PickUpObject() 
{
	if (!objectPicked) {
		
		FVector Start = GetFollowCamera()->GetComponentLocation();
		FVector End = Start + (GetFollowCamera()->GetForwardVector() * PickUpDistance);
		FHitResult Hit;

		FCollisionQueryParams TraceParam;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParam))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("You are hitting: %s"), *Hit.GetActor()->GetName()));
			DrawDebugLine(GetWorld(), Start, Hit.Location, FColor::Green, false, 1.5f);
			if (Hit.GetComponent()->Mobility == EComponentMobility::Movable)
			{
				objectPicked = true;
				Object = Hit.GetActor();
				Object->FindComponentByClass<UStaticMeshComponent>()->SetSimulatePhysics(false);
				Hit.GetActor()->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform);
				Hit.GetActor()->SetActorLocation(GetCapsuleComponent()->GetRelativeLocation() + (GetCapsuleComponent()->GetForwardVector() * 150));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("You aren't hitting something"));
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.5f);
		}
	}
	else {
		DropObject();
	}
}

void AExo_CPlusPlusCharacter::DropObject() 
{
	if (Object) {
		Object->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Object->FindComponentByClass<UStaticMeshComponent>()->SetSimulatePhysics(true);
		Object = NULL;
		objectPicked = false;
	}
}

void AExo_CPlusPlusCharacter::Shoot() 
{
	FVector Location;
	FRotator Rotation;
	FActorSpawnParameters SpawnInfo;

	Location = GetCapsuleComponent()->GetRelativeLocation() + (GetCapsuleComponent()->GetForwardVector() * 100);
	Rotation = GetCapsuleComponent()->GetComponentRotation();

	GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(), Location, Rotation, SpawnInfo);
}

void AExo_CPlusPlusCharacter::SaveGame() 
{
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	SaveGameInstance->PlayerPosition = this->GetActorTransform();
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Game Saved"));
}

void AExo_CPlusPlusCharacter::LoadGame() 
{
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0));
	this->SetActorLocation(SaveGameInstance->PlayerPosition.GetLocation());
	this->SetActorRotation(SaveGameInstance->PlayerPosition.GetRotation());
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Game Load"));
}