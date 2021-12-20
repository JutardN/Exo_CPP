// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Exo_CPlusPlusCharacter.generated.h"

UCLASS(config=Game)
class AExo_CPlusPlusCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AExo_CPlusPlusCharacter();

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	int health=100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	int maxHealth=100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	int strength=1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	int defense=1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	int gold=0;

	UPROPERTY(VisibleAnywhere)
	bool isCrouching;

	UPROPERTY(EditAnywhere)
	float PickUpDistance = 500.0f;

	bool objectPicked=false;
	AActor* Object;

	bool isDead;
	
	UFUNCTION()
	void Death();

	void PickUpObject();
	void DropObject();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SaveGame();
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void LoadGame();

	void StartRun();
	void StopRun();

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	FTimerHandle RespawnHandle;
private:
	void StartCrouch();
	void StopCrouching();

	void Shoot();
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

