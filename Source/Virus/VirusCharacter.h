// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "VirusCharacter.generated.h"


UCLASS(config=Game)
class AVirusCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Look Rate while not Aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0",ClampMax = "1.0",UIMin = "0.0",UIMax = "1.0"))
	float HipLookRate;

	/** Look Rate while Aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookRate;


	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ScanAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* HealAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimingAction;

	/** Montage for double jump */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DoubleJumpMontage;

	/** Scan Shot Sound cue*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/** Flash spawned at BarrelSocket */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* LaserFlash;

	/** Montage for attack using the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ScaningMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* BeamParticles;

	/** True when aiming*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** Default camera field of view value */
	float CameraDefaultFOV;

	/** Field of view value for when zoomed in */
	float CameraZoomedFOV;

	/** Current field of view this frame */
	float CameraCurrentFOV;


	/** Interp speed for zooming when aiming */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/** Flash spawned at BarrelSocket */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Healing, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* HealingVFX;

	/** Montage for attack using the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Healing, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HealingMontage;


	/** MinimapArm positioning the camera upon the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Minimap, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* MinimapArm;

	/** Follow camera for minimap */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Minimap, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* MinimapCapture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	bool bisDoubleJump;

	/** Determines the spread of the crosshairs */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	
	/** Velocity component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/** In air component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/** Aim component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CroshairAimFactor;

	/** Shooting component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;




public:
	AVirusCharacter();

	UFUNCTION(BlueprintCallable)
	float GetHP();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AVirusPlayerController* VirusPlayerController;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for movement input */
	void DoubleJump(const FInputActionValue& Value);

	/** Called for Shooting input */
	void Scan(const FInputActionValue& Value);

	/** Called for Healing input */
	void Heal(const FInputActionValue& Value);

	void Aiming(const FInputActionValue& Value);
	void StopAiming(const FInputActionValue& Value);

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	void CameraInterpZoom(float DeltaTime);

	void CalculateCrosshairSpread(float DeltaTime);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	class UNiagaraComponent* EndHitInstance;
	class UNiagaraComponent* BeamInstance;
	class UNiagaraComponent* FlashInstance;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	FORCEINLINE bool GetDoubleJump() const { return bisDoubleJump; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

};

