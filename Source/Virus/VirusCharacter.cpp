// Copyright Epic Games, Inc. All Rights Reserved.

#include "VirusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "AIProgramCharacter.h"
#include "AIVaccineCharacter2.h"
#include "BulletHitInterface.h"
#include "VirusPlayerController.h"
#include "VirusGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraEmitter.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "Heal.h"
#include "BroadHacking.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "AttackItem.h"
#include "AIOperatingSystem.h"
#include "Misc/OutputDeviceNull.h"
#include "Particles/ParticleSystemComponent.h"
#include "ItemBox.h"
#include "BroadHackingSkill.h"
//////////////////////////////////////////////////////////////////////////
// AVirusCharacter



AVirusCharacter::AVirusCharacter() :
	bAiming(false),
	MaxHP(100.0f),
	bisDoubleJump(false),

	CameraDefaultFOV(0.f), //set in BeginPlay
	CameraZoomedFOV(50.f),
	CameraZoomedOutFOV(90.f),
	CameraCurrentFOV(50.f),
	ZoomInterpSpeed(20.f),

	//Turn rates for aiming/not aiming
	HipLookRate(1.0f),
	AimingLookRate(0.4f),

	bShouldTraceForItems(false),
	OverlappedItemCount(0),

	//Camerainterp location variables
	CameraInterpDistance(150.f),
	CameraInterpElevation(45.f),
	bShouldPlayPickupSound(true),
	bisScanning(false),

	bCanUseHeal(true),
	HealCoolTime(5.f),
	BroadHackinglCoolTime(5.f),
	bCanUseBroadHacking(true),
	CombatState(ECombatState::ECS_Normal),
	DefenseStatus(EDefenseStatus::EDS_DeActivate),
	bAttackDefense(false),
	KeyIndex(-1),
	bWarningWidget(true),
	bBoxOpening(false)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Charactser moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraBoom->SocketOffset = FVector(0.f, 20.f, 50.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	MinimapArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MinimapArm"));
	MinimapArm->SetupAttachment(RootComponent);
	MinimapArm->TargetArmLength = 3000.0f; // The camera follows at this distance behind the character	
	MinimapArm->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	MinimapArm->bInheritPitch = false;
	MinimapArm->bInheritYaw = false;
	MinimapArm->bInheritRoll = false;

	MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	MinimapCapture->SetupAttachment(MinimapArm, USpringArmComponent::SocketName);
	MinimapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MinimapCapture->OrthoWidth = 3000.0f;

	DefenseArea = CreateDefaultSubobject<USphereComponent>(TEXT("DefenseArea"));
	DefenseArea->SetupAttachment(GetMesh());

	DefenseSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefenseSphere"));
	DefenseSphere->SetupAttachment(DefenseArea);


	/*static ConstructorHelpers::FClassFinder<UUserWidget> SkillAsset(TEXT("/Game/_VirusGame/HUD/BP_SkillWidget.BP_SkillWidget_C"));
	
	if (SkillAsset.Succeeded()) {
		SkillClass = SkillAsset.Class;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("there is no SkillAsset"));
	}*/
}

void AVirusCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	AnimInstance = GetMesh()->GetAnimInstance();

	VirusPlayerController = Cast<AVirusPlayerController>(GetController());
	CurrentHP = MaxHP;

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	
	JumpMaxCount = 1;
	//Spawn the default weapon and attach it to the mesh
	EquipWeapon(SpawnDefaultWeapon());
	EquippedWeapon->DisableCustomDepth();

	UVirusGameInstance* VirusInstance = Cast<UVirusGameInstance>(GetGameInstance());

	HipLookRate = 0.3f * VirusInstance->MouseSensitivity;
	AimingLookRate = 0.1f * VirusInstance->MouseSensitivity;

	SetDefenseProperties(DefenseStatus);

	/*TArray<UUserWidget*> FindSkillWidget;

	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FindSkillWidget, SkillClass);

	if (FindSkillWidget[0]) {
		SkillWidget = FindSkillWidget[0];
		UE_LOG(LogTemp, Warning, TEXT("Get SkillWidget"));
	}
	else UE_LOG(LogTemp, Warning, TEXT("There is no SkillWidget"));*/

	if (UIManager) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), UIManager, FoundActors);
		if (FoundActors.Num() > 0) {
			UIMN = FoundActors[0];

			/*const FString command = FString::Printf(TEXT("GameClear"));
			FOutputDeviceNull Ar;

			UIMN->CallFunctionByNameWithArguments(*command, Ar, NULL, true); When Die code*/
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UIMN Null"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("UIManager Null"));
	}
}

float AVirusCharacter::GetHP()
{
	return CurrentHP / MaxHP;
}

void AVirusCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Normal;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

float AVirusCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	if (CurrentHP - DamageAmount <= 0.f)
	{
		CurrentHP = 0.f;
		Die();
		/*
		auto VaccineController = Cast<AAIController>(EventInstigator);
		if (VaccineController)
		{
			if (VaccineController->GetBlackboardComponent())
			{
				GEngine->AddOnScreenDebugMessage(11, 3.0f, FColor::Magenta, FString::Printf(TEXT("Vaccine")));
				VaccineController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("CharacterDead")), true);
			}
			
		}
		*/

	}
	else
	{
		CurrentHP -= DamageAmount;
	}

	return DamageAmount;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AVirusCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AVirusCharacter::DoubleJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVirusCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVirusCharacter::Look);

		//Scanning
		EnhancedInputComponent->BindAction(ScanAction, ETriggerEvent::Started, this, &AVirusCharacter::Scan);
		EnhancedInputComponent->BindAction(ScanAction, ETriggerEvent::Completed, this, &AVirusCharacter::StopScan);

		//Heal
		EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Started, this, &AVirusCharacter::Heal);

		//Aiming
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Started, this, &AVirusCharacter::Aiming);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AVirusCharacter::StopAiming);

		//Select
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &AVirusCharacter::Select);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Completed, this, &AVirusCharacter::StopSelect);

		//Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AVirusCharacter::Reload);

		//Using Item from the Inventory Slot
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &AVirusCharacter::GetInventory);

		//BroadHacking
		EnhancedInputComponent->BindAction(BroadHackingAction, ETriggerEvent::Started, this, &AVirusCharacter::BroadHacking);

	}

}

void AVirusCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AVirusCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	float LookScaleX, LookScaleY;

	
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		if (bAiming)
		{
			LookScaleX = LookAxisVector.X * AimingLookRate;
			LookScaleY = LookAxisVector.Y * AimingLookRate;
		}
		else
		{
			LookScaleX = LookAxisVector.X * HipLookRate;
			LookScaleY = LookAxisVector.Y * HipLookRate;
		}
		AddControllerYawInput(LookScaleX);
		AddControllerPitchInput(LookScaleY);
	}
}

void AVirusCharacter::DoubleJump(const FInputActionValue& Value)
{

	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;

	bisDoubleJump = JumpCurrentCount ? true : false;
	
	if (CanJump() && bisDoubleJump)
	{
		if (AnimInstance && DoubleJumpMontage)
		{
			FRotator CharacterRotation = GetActorRotation();
			FVector LaunchDirection = CharacterRotation.Vector();
			LaunchDirection.Normalize(); // vector normallize
			float LaunchStrength = 25.0f;
			LaunchDirection.Z = 20.0f;

			AnimInstance->Montage_Play(DoubleJumpMontage);
			AnimInstance->Montage_JumpToSection(FName("DoubleJump"));

			LaunchCharacter(LaunchDirection * LaunchStrength, false, false);
		}

	}
}


void AVirusCharacter::AttackWeapon(float DeltaTime)
{
	if (!bisScanning) return;
	if (EquippedWeapon == nullptr) return;


	if (CombatState == ECombatState::ECS_Normal)
	{
		if (CheckReloading())
		{
			CombatState = ECombatState::ECS_Reloading;
			PlayReloadMontage();
		}

		if (WeaponHasGage())
		{
			if (Controller != nullptr && FireSound)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Attack"));

				UGameplayStatics::PlaySound2D(this, FireSound);
				if (AnimInstance && ScaningMontage)
				{
					AnimInstance->Montage_Play(ScaningMontage);
					AnimInstance->Montage_JumpToSection(FName("Attack"));
				}
			}

			/* Legacy */
			//const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

			/* New */
			const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

			if (BarrelSocket)
			{

				//FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh(), ERelativeTransformSpace::RTS_World);
				FTransform SocketTransform = EquippedWeapon->GetItemMesh()->GetSocketTransform(FName("BarrelSocket"), ERelativeTransformSpace::RTS_World);

				LaserFlash = EquippedWeapon->GetLaserFlash();

				//Spawn LaserFlash
				if (LaserFlash)
				{
					UNiagaraComponent* FlashInstance = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LaserFlash, SocketTransform.GetLocation());

					FlashInstance->Activate();
				}

				FHitResult BeamHitResult;
				bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);
				if (bBeamEnd)
				{
					// Does hit Actor implement BulletHitInterface?
					if (BeamHitResult.GetActor())
					{
						IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.GetActor());
						if (BulletHitInterface)
						{
							BulletHitInterface->BulletHit_Implementation(BeamHitResult);
						}

						AAIProgramCharacter* HitProgram = Cast<AAIProgramCharacter>(BeamHitResult.GetActor());
						AAIVaccineCharacter2* HitVaccine = Cast<AAIVaccineCharacter2>(BeamHitResult.GetActor());
						AAIOperatingSystem* HitOS = Cast<AAIOperatingSystem>(BeamHitResult.GetActor());

						if (HitProgram)
						{
							//Head Shot
							if (BeamHitResult.BoneName.ToString() == HitProgram->GetHeadBone())
							{
								if (HitProgram->Health > 0.f)
								{
									UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), HeadShotDamage, GetController(), this, UDamageType::StaticClass());
								}
							}
							//Body Shot
							else
							{
								if (HitProgram->Health > 0.f)
								{
									UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), BodyShotDamage, GetController(), this, UDamageType::StaticClass());
								}
							}
						}
						else if (HitVaccine) {
							//Head Shot
							if (BeamHitResult.BoneName.ToString() == HitVaccine->GetHeadBone())
							{
								if (HitVaccine->Health > 0.f)
								{
									UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), HeadShotDamage, GetController(), this, UDamageType::StaticClass());
								}
							}
							//Body Shot
							else
							{
								if (HitVaccine->Health > 0.f)
								{
									UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), BodyShotDamage, GetController(), this, UDamageType::StaticClass());
								}
							}

						}
						else if (HitOS) {
							if (HitOS->Health > 0.f) {
								HitOS->Health -= 10.f;

							}
							else {
								HitOS->Die();
							}
						}
						else
						{
							ImpactParticles = EquippedWeapon->GetImpactParticles();
							//Spawn default particles
							if (ImpactParticles)
							{
								UNiagaraComponent* EndHitInstance = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactParticles, BeamHitResult.Location);
								EndHitInstance->Activate();
							}
						}
					}

					BeamParticles = EquippedWeapon->GetBeamParticles();
					if (BeamParticles)
					{
						UNiagaraComponent* BeamInstance = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BeamParticles, SocketTransform.GetLocation());

						if (BeamInstance)
						{
							BeamInstance->SetVectorParameter(FName("LaserStart"), SocketTransform.GetLocation());
							BeamInstance->SetVectorParameter(FName("LaserEnd"), BeamHitResult.Location);

							BeamInstance->Activate();
						}

					}

				}

			}
		}
	}

}

void AVirusCharacter::Scan(const FInputActionValue& Value)
{
	if (EquippedWeapon)
	{
		bisScanning = true;
	}

}

void AVirusCharacter::StopScan(const FInputActionValue& Value)
{
	if (EquippedWeapon)
	{
		bisScanning = false;
	}
}

bool AVirusCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector2D ViewportSize;
	FVector OutBeamLocation;

	//Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		// Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else //no crosshair trace hit
	{
		//OutBeamLocation is the End location for the line trace
	}

	//perform a second trace, this time from the gun barrel
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - WeaponTraceStart };
	const FVector WeaponTraceEnd{ WeaponTraceStart + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

	if (OutHitResult.bBlockingHit) // object between barrel and BeamEndpoint?
	{
		OutHitResult.Location = OutBeamLocation;
		//DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponTraceEnd, FColor::Red, false, 2.f);
		return true;
	}

	return false;
}

void AVirusCharacter::Heal(const FInputActionValue& Value)
{

	if (bCanUseHeal)
	{
		bCanUseHeal = false;

		const FString command = FString::Printf(TEXT("E_Cooldown"));
		FOutputDeviceNull Ar;

		if (UIMN) UIMN->CallFunctionByNameWithArguments(*command, Ar, NULL, true);
		else {
			UE_LOG(LogTemp, Warning, TEXT("There is no UIMN"));
			return;
		}
		
		if (AnimInstance && SkillMontage)
		{
			AnimInstance->Montage_Play(SkillMontage);
			AnimInstance->Montage_JumpToSection("Heal");
		}

		float AnimPlayTime = 0.7f;

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AVirusCharacter::SpawnHealPackAfterAnim, AnimPlayTime);
	}
}

void AVirusCharacter::Select(const FInputActionValue& Value)
{
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this);
		TraceHitItem = nullptr;
	}

	if (TraceHitItemBox)
	{
		if (GetKeyEnough() && TraceHitItemBox->isOpened() == false)
		{
			TempInventory["Key"] -= 2;
			if (TempInventory["Key"] == 0)
			{
				TempInventory.Remove("Key");
				Inventory.RemoveAt(KeyIndex);
			}
			TraceHitItemBox->PlayAnimMontage();
			TraceHitItemBox->GetCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			bBoxOpening = true;
		}
	}
}

void AVirusCharacter::StopSelect(const FInputActionValue& Value)
{

}

void AVirusCharacter::Aiming(const FInputActionValue& Value)
{
	
	if (CombatState != ECombatState::ECS_Reloading && CombatState != ECombatState::ECS_Stunned)
	{
		bAiming = true;
	}
}

void AVirusCharacter::StopAiming(const FInputActionValue& Value)
{
	bAiming = false;
}

void AVirusCharacter::Reload(const FInputActionValue& Value)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		bAiming = false;
		return;
	}

	if (CombatState == ECombatState::ECS_Normal)
	{
		CombatState = ECombatState::ECS_Reloading;

		if (!CheckReloading())
		{
			EquippedWeapon->SetWeapongageStatus(EWeapongageStatus::EWS_Reloading);
			PlayReloadMontage();
		}
	}
	
}

void AVirusCharacter::GetInventory(const FInputActionValue& Value)
{
	float ItemKey = Value.Get<float>();
	
	if (ItemKey <= Inventory.Num())
	{
		AItem* GetItem = Cast<AItem>(Inventory[ItemKey - 1]);
		FString ItemName = GetItem->GetItemName();
		EItemType ItemType = GetItem->GetItemType();

		if (ItemType == EItemType::EIT_Key) return;
		TempInventory[ItemName]--;

		if (TempInventory[ItemName] == 0)
		{
			TempInventory.Remove(ItemName);
			Inventory.RemoveAt(ItemKey - 1);
		}
		UseItem(ItemType,GetItem);
	}

}

void AVirusCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
		CameraBoom->TargetArmLength = 50.0f;
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
		CameraBoom->TargetArmLength = 200.0f;
	}

	/*
	* if (bAttackDefense || bCCTVDefense)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedOutFOV, DeltaTime, ZoomInterpSpeed);
		CameraBoom->TargetArmLength = 500.0f;
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
		CameraBoom->TargetArmLength = 200.0f;
	}
	*/
	

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	
}

void AVirusCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, GetCharacterMovement()->MaxWalkSpeed };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector2D Velocity{ GetVelocity() };

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor;
}

bool AVirusCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	//Get Viewport Size
	FVector2D ViewportSize;

	//GEngine -> global engine pointer: hold viewport
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		//Trace from Crosshair world location outward
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;

		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}

	}
	return false;
}

void AVirusCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			TraceHitItemBox = Cast<AItemBox>(ItemTraceResult.GetActor());
			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickUpWidget() && TraceHitItem->IsOverlappingActor(this))
			{
				//Show Item's PickupWidget
				TraceHitItem->GetPickUpWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();
			}

			if (TraceHitItemBox && !bBoxOpening && TraceHitItemBox->GetPickUpWidget() && TraceHitItemBox->GetWarningWidget() && TraceHitItemBox->IsOverlappingActor(this))
			{
				//Show Item's PickupWidget
				if (GetKeyEnough())
				{
					TraceHitItemBox->GetPickUpWidget()->SetVisibility(true);
					TraceHitItemBox->GetWarningWidget()->SetVisibility(false);
				}
				else
				{
					TraceHitItemBox->GetPickUpWidget()->SetVisibility(false);
					TraceHitItemBox->GetWarningWidget()->SetVisibility(true);
				}
				TraceHitItemBox->EnableCustomDepth();
			}

			// We hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//We are hitting a different AItem this frame from last frame Or AItem is null
					TraceHitItemLastFrame->GetPickUpWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}

			// We hit an AItem last frame
			if (TraceHitItemBoxLastFrame)
			{
				if (TraceHitItemBox != TraceHitItemBoxLastFrame)
				{
					//We are hitting a different AItem this frame from last frame Or AItem is null
					TraceHitItemBoxLastFrame->GetPickUpWidget()->SetVisibility(false);
					TraceHitItemBoxLastFrame->GetWarningWidget()->SetVisibility(false);
					TraceHitItemBoxLastFrame->DisableCustomDepth();
				}
			}
			//store a reference to HitItem for next frame
			TraceHitItemLastFrame = TraceHitItem;
			TraceHitItemBoxLastFrame = TraceHitItemBox;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		//No longer overlapping any items,
		//Items last frame should not show widget
		TraceHitItemLastFrame->GetPickUpWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
	else if (TraceHitItemBoxLastFrame)
	{
		//No longer overlapping any items,
		//Items last frame should not show widget
		TraceHitItemBoxLastFrame->GetPickUpWidget()->SetVisibility(false);
		TraceHitItemBoxLastFrame->GetWarningWidget()->SetVisibility(false);
		TraceHitItemBoxLastFrame->DisableCustomDepth();
	}
}

AWeapon* AVirusCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		//Spawn the Weapon
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		EquippedWeapon->SetItemType(EItemType::EIT_Weapon);
		return EquippedWeapon;
	}

	return nullptr;
}

void AVirusCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		//Set AreaSphere to ignore all Collision Channels
		WeaponToEquip->GetAreaSphere()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		//Set CollisionBox to ignore all Collision Channels
		WeaponToEquip->GetCollisionBox()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		//Get the Hand Socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		
		if (HandSocket)
		{
			//Attach the Weapon the hand socket RightHandSocket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
			CombatState = ECombatState::ECS_Normal;
		}
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AVirusCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AVirusCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

bool AVirusCharacter::WeaponHasGage()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetGageAmount() > 0.f;
}

bool AVirusCharacter::CheckReloading()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetWeapongageStatus() == EWeapongageStatus::EWS_Reloading;

}

void AVirusCharacter::PlayReloadMontage()
{
	if (AnimInstance && ScaningMontage)
	{
		AnimInstance->Montage_Play(ScaningMontage);
		AnimInstance->Montage_JumpToSection("Reload");
		/* New */
		const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

		if (BarrelSocket && ReloadVFX)
		{
			//FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh(), ERelativeTransformSpace::RTS_World);
			FTransform SocketTransform = EquippedWeapon->GetItemMesh()->GetSocketTransform(FName("BarrelSocket"), ERelativeTransformSpace::RTS_World);
			UGameplayStatics::SpawnEmitterAttached(ReloadVFX, EquippedWeapon->GetItemMesh(), NAME_None, SocketTransform.GetLocation(), FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition);
			
			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ReloadVFX, SocketTransform);
		}
	}
}

void AVirusCharacter::UseItem(EItemType Type, AItem* Item)
{
	switch (Type)
	{
		case EItemType::EIT_AttackItem:
		{
			if (AttackItemClass)
			{
				//Spawn the HealPack at Character Socket position

				const USkeletalMeshSocket* ItemSocket = GetMesh()->GetSocketByName("Heal");

				if (ItemSocket)
				{
					FTransform ItemSocketTransform = ItemSocket->GetSocketTransform(GetMesh());
					AAttackItem* AttackItem = GetWorld()->SpawnActor<AAttackItem>(AttackItemClass, ItemSocketTransform);

					if (AttackItem)
					{
						FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
						AttackItem->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

						AttackItem->SetAttackItemStatus(EAttackItemStatus::EAIS_Falling);
						AttackItem->ThrowItem();
					}
				}

			}

			break;
		}

		case EItemType::EIT_AttackDefenseItem:
		{
			SetDefenseStatus(EDefenseStatus::EDS_Activate);
			float AttackDefenseTime = Item->GetItemDuration();
			
			bAttackDefense = true;
			GetWorldTimerManager().SetTimer(AttackDefenseItemTimer, this, &AVirusCharacter::FinishAttackDefense, AttackDefenseTime);

			break;
		}
		case EItemType::EIT_CCTVDefenseItem:
		{
			SetDefenseStatus(EDefenseStatus::EDS_Activate);
			float CCTVDefenseTime = Item->GetItemDuration();

			bCCTVDefense = true;
			GetWorldTimerManager().SetTimer(CCTVDefenseItemTimer, this, &AVirusCharacter::FinishCCTVDefense, CCTVDefenseTime);

			break;
		}
		
		case EItemType::EIT_SpeedItem:
		{
			GetCharacterMovement()->MaxWalkSpeed *= 3.0f;
			float OverClockTime = Item->GetItemDuration();
			GetWorldTimerManager().SetTimer(OverClockTimer, this, &AVirusCharacter::FinishOverClock, OverClockTime);
			break;
		}
		
	}
}

AHeal* AVirusCharacter::SpawnHealPack()
{
	if (HealClass)
	{
		//Spawn the HealPack at Character Socket position

		const USkeletalMeshSocket* HealPackSocket = GetMesh()->GetSocketByName("Heal");

		if (HealPackSocket)
		{
			FTransform HealSocketTransform = HealPackSocket->GetSocketTransform(GetMesh());
			return GetWorld()->SpawnActor<AHeal>(HealClass, HealSocketTransform);
		}
	}
	return nullptr;
}

void AVirusCharacter::SpawnBroadHackingAfterAnim()
{
	const USkeletalMeshSocket* BroadHackingSocket = GetMesh()->GetSocketByName("Heal");

	if (BroadHackingSocket)
	{
		FTransform BroadHackingSocketTransform = BroadHackingSocket->GetSocketTransform(GetMesh());

		ABroadHackingSkill* AttackRange = GetWorld()->SpawnActor<ABroadHackingSkill>(DefaultBroadHackingClass, BroadHackingSocketTransform);

		if (AttackRange)
		{
			FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
			AttackRange->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

			AttackRange->SetBroadHackingStatus(EBroadHackingSkillStatus::EBHSS_Falling);
			AttackRange->ThrowItem(GetCharacterMovement()->Velocity.Size());

			GetWorldTimerManager().SetTimer(BroadHackingCoolTimer, this, &AVirusCharacter::CanUseBroadHacking, BroadHackinglCoolTime);
		}

	}
}


void AVirusCharacter::HealPackOverlap(float DeltaTime)
{
	if (SpawnedHealPack == nullptr) return;

	if (SpawnedHealPack->GetHealOverlap())
	{

		float DeltaHP = 20.0f * DeltaTime;

		if (CurrentHP <= MaxHP)
		{
			CurrentHP += DeltaHP;
		}
		else
		{
			CurrentHP = MaxHP;
		}

	}
	
}

void AVirusCharacter::BroadHacking()
{
	if (bCanUseBroadHacking)
	{
		bCanUseBroadHacking = false;
		
		const FString command = FString::Printf(TEXT("Q_Cooldown"));
		FOutputDeviceNull Ar;

		UIMN->CallFunctionByNameWithArguments(*command, Ar, NULL, true);

		if (AnimInstance && SkillMontage)
		{
			AnimInstance->Montage_Play(SkillMontage);
			AnimInstance->Montage_JumpToSection("BroadHacking");
		}
		
		if (BroadHackingVFX)
		{
			UGameplayStatics::SpawnEmitterAttached(BroadHackingVFX, GetCapsuleComponent(),NAME_None,GetCapsuleComponent()->GetRelativeLocation(),FRotator::ZeroRotator,EAttachLocation::KeepWorldPosition);
		}
		
		
		float AnimPlayTime = 0.7f;

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AVirusCharacter::SpawnBroadHackingAfterAnim, AnimPlayTime);

	}
	
}

void AVirusCharacter::CanUseHeal()
{
	bCanUseHeal = true;
}

void AVirusCharacter::CanUseBroadHacking()
{
	bCanUseBroadHacking = true;
}

void AVirusCharacter::FinishOverClock()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AVirusCharacter::SpawnHealPackAfterAnim()
{
	SpawnedHealPack = SpawnHealPack();

	if (SpawnedHealPack)
	{

		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		SpawnedHealPack->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		SpawnedHealPack->SetHealStatus(EHealStatus::EHS_Falling);
	
		SpawnedHealPack->ThrowHealPack(GetCharacterMovement()->Velocity.Size());
	}
	
	GetWorldTimerManager().SetTimer(HealCoolTimer, this, &AVirusCharacter::CanUseHeal, HealCoolTime);
}

void AVirusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle interpolation for zoom when aiming
	CameraInterpZoom(DeltaTime);
	//Calculate Crosshair Spread Multiplier
	CalculateCrosshairSpread(DeltaTime);

	//Check OverlappedItemCount, then trace for items
	TraceForItems();

	AttackWeapon(DeltaTime);

	HealPackOverlap(DeltaTime);

	
	/* Set Equipped Weapon Relative World Scale default */
	if (GEngine && EquippedWeapon)
	{
		FVector WeaponScale = EquippedWeapon->GetActorRelativeScale3D();
		//이유를 모르겠다...ㅜㅜㅜ ㅜㅠ
		EquippedWeapon->SetActorRelativeScale3D(FVector(1.0f));
		//FString Message = FString::Printf(TEXT("Equipped Weapon Scale : %f %f %f"), WeaponScale.X, WeaponScale.Y, WeaponScale.Z);
		//GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, Message);
	}
}

void AVirusCharacter::PlayStunMontage()
{
	if (AnimInstance && HitReactMontage)
	{
		
		AnimInstance->Montage_Play(HitReactMontage);
		GetCharacterMovement()->MaxWalkSpeed = 100.f;
		if (HitVFX)
		{
			UGameplayStatics::SpawnEmitterAttached(HitVFX, GetCapsuleComponent(), NAME_None, GetCapsuleComponent()->GetRelativeLocation(), FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition);
		}
	}
}

void AVirusCharacter::Die()
{
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection("Death");
	}
}

void AVirusCharacter::SetDefenseProperties(EDefenseStatus State)
{
	switch (State)
	{
	case EDefenseStatus::EDS_Activate:

		DefenseArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		DefenseArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		DefenseSphere->SetVisibility(true);
		DefenseSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		DefenseSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;

	case EDefenseStatus::EDS_DeActivate:
	
		DefenseArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		DefenseArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		DefenseSphere->SetVisibility(false);
		DefenseSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		DefenseSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	}
}

void AVirusCharacter::FinishAttackDefense()
{
	SetDefenseStatus(EDefenseStatus::EDS_DeActivate);
	bAttackDefense = false;
}

void AVirusCharacter::FinishCCTVDefense()
{
	SetDefenseStatus(EDefenseStatus::EDS_DeActivate);
	bCCTVDefense = false;
}

bool AVirusCharacter::GetKeyEnough()
{
	for (int i = 0; i < Inventory.Num(); i++)
	{
		auto GetItem = Inventory[i];
		if (GetItem->GetItemType() == EItemType::EIT_Key)
		{
			GEngine->AddOnScreenDebugMessage(22, 3.f, FColor::Cyan, FString::Printf(TEXT("KeyIndex: %d"), KeyIndex));
			KeyIndex = i;
			FString ItemName = GetItem->GetItemName();
			if(TempInventory[ItemName] >= 2)
			{
				return true;
			}
		}
	}
	return false;
}

float AVirusCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AVirusCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

FVector AVirusCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForward{ FollowCamera->GetForwardVector() };

	//Desired = CameraWorldLocation + Forward*A + Up*B
	return CameraWorldLocation + CameraForward * CameraInterpDistance
		+ FVector(0.f, 0.f, CameraInterpElevation);
}

void AVirusCharacter::GetPickUpItem(AItem* Item)
{
	auto GetItem = Cast<AItem>(Item);
	auto Weapon = Cast<AWeapon>(Item);

	if (Weapon)
	{
		SwapWeapon(Weapon);
	}
	else
	{
		EItemType ItemType = GetItem->GetItemType();

		if (ItemType != EItemType::EIT_DoubleJump)
		{

			if (!TempInventory.Contains(GetItem->GetItemName()))
			{
				if (Inventory.Num() < INVENTORY_CAPACITY)
				{
					GetItem->SetSlotIndex(Inventory.Num());
					Inventory.Add(GetItem);
					TempInventory.Add(GetItem->GetItemName(), GetItem->GetItemCount());

				}
				
			}
			else
			{
				TempInventory[GetItem->GetItemName()] += GetItem->GetItemCount();
			}

			int32 value = TempInventory[GetItem->GetItemName()];

			FString Message = FString::Printf(TEXT("Get Item Count : %d "), value);
			GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::White, Message);
			GetItem->SetItemState(EItemState::EIS_PickedUp);



			// Play Widget Anim
			/*if (TempInventory.Num() == 0)
			{
				EquipItemDelegate.Broadcast(-1, GetItem->GetSlotIndex());
			}
			else
			{
				EquipItemDelegate.Broadcast((GetItem->GetSlotIndex()) - 1, GetItem->GetSlotIndex());
				//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::White, FString::Printf(TEXT("CurrentIndex: %d, NextIndex: %d"), GetItem->GetSlotIndex(), TempInventory.Num()));
			}
			*/
		}
		else
		{
			JumpMaxCount = 2;
		}
		
	}
}


void AVirusCharacter::Stun()
{
	//if (CurrentHP <= 0.f) return;
	CombatState = ECombatState::ECS_Stunned;
	PlayStunMontage();
	//FTimerHandle DelayAnim;
	//GetWorldTimerManager().SetTimer(DelayAnim, this, &AVirusCharacter::PlayStunMontage, 0.6f);

}

void AVirusCharacter::EndReload()
{
	CombatState = ECombatState::ECS_Normal;
	EquippedWeapon->SetWeapongageStatus(EWeapongageStatus::EWS_Normal);
}

void AVirusCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}
}

void AVirusCharacter::SetDefenseStatus(EDefenseStatus Status)
{
	DefenseStatus = Status;
	SetDefenseProperties(DefenseStatus);
}

void AVirusCharacter::EndOpen()
{
	bBoxOpening = false;
}

