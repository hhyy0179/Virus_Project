// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "VirusCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

UCLASS(config = Game)
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SelectAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimInstance* AnimInstance;

	/** Montage for double jump */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DoubleJumpMontage;

	/** Flash spawned at BarrelSocket */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* HealingVFX;

	/** Montage for healing action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HealingMontage;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	bool bisScanning;

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

	/** True if we should trace every frame for items */
	bool bShouldTraceForItems;

	/** Number of overlapped AItems */
	int8 OverlappedItemCount;

	/** Currently eqipped Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquippedWeapon;

	/** Set this in Blurprints for the default weapon class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/** The item currently hit by out trace in TraceForItems (could be null) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItem;

	/** The AItem we hit last frame */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItemLastFrame;

	/** Distance upward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	/** Distance upward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	FTimerHandle PickupSoundTimer;
	bool bShouldPlayPickupSound;
	void ResetPickupSoundTimer();

	/** Time to wait before we can play another Pickup Sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;

	/** An array of AItem for our Inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	const int32 INVENTORY_CAPACITY{ 5 };

	/** An Map of AItem for our Inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TMap<AItem*, int32> TempInventory;


	/** Delegate for sending slot information to InventoryBar when equipping */
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	bool bReloading;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for movement input */
	void DoubleJump(const FInputActionValue& Value);

	/** Called for Shooting input */
	void Scan(const FInputActionValue& Value);
	void StopScan(const FInputActionValue& Value);

	/** Called for Healing input */
	void Heal(const FInputActionValue& Value);

	/** Called for Select input */
	void Select(const FInputActionValue& Value);
	void StopSelect(const FInputActionValue& Value);

	void Aiming(const FInputActionValue& Value);
	void StopAiming(const FInputActionValue& Value);

	void Reload(const FInputActionValue& Value);

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	void AttackWeapon(float DeltaTime);

	void CameraInterpZoom(float DeltaTime);

	void CalculateCrosshairSpread(float DeltaTime);

	/** Line trace for items under the crosshairs */
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/** Trace for items if OverlappedItemCounts */
	void TraceForItems();

	/** Spawns a default weapon and equips it */
	AWeapon* SpawnDefaultWeapon();

	/** Takes a weapon and attaches it to the mesh */
	void EquipWeapon(AWeapon* WeaponToEquip);

	/**Detach weapon and let it fall to the ground */
	void DropWeapon();

	/** Drops currently equipped Weapon and Equips TraceHitItem */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/**Check to make sure our weapon has gage  */
	bool WeaponHasGage();

	bool CheckReloading();

	void PlayReloadMontatge();

	void OnReloadMontageEnded();

public:
	AVirusCharacter();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	float GetHP();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AVirusPlayerController* VirusPlayerController;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	FORCEINLINE bool GetDoubleJump() const { return bisDoubleJump; }

	FORCEINLINE bool GetScanning() const { return bisScanning; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/** Adds/ Subtracts to/from Overlapped ItemCount and updates bShouldTrace for Items */
	void IncrementOverlappedItemCount(int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickUpItem(AItem* Item);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }

private:
	float HeadShotDamage = 0.5f;
	float BodyShotDamage = 0.1f;

};

