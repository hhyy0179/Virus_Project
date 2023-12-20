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
#include "BulletHitInterface.h"
#include "VirusPlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

//////////////////////////////////////////////////////////////////////////
// AVirusCharacter



AVirusCharacter::AVirusCharacter(): 
	bAiming(false),
	MaxHP(100.0f),
	bisDoubleJump(false),

	CameraDefaultFOV(0.f), //set in BeginPlay
	CameraZoomedFOV(50.f),
	CameraCurrentFOV(50.f),
	ZoomInterpSpeed(20.f),

	//Turn rates for aiming/not aiming
	HipLookRate(1.f),
	AimingLookRate(0.4f)

{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

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

	VirusPlayerController = Cast<AVirusPlayerController>(GetController());
	CurrentHP = MaxHP;

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

}

float AVirusCharacter::GetHP()
{
	return CurrentHP / MaxHP;
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

		//Heal
		EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Started, this, &AVirusCharacter::Heal);

		//Aiming
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Started, this, &AVirusCharacter::Aiming);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AVirusCharacter::StopAiming);

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
	float LookScaleX , LookScaleY;
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
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && DoubleJumpMontage)
		{
			FRotator CharacterRotation = GetActorRotation(); 
			FVector LaunchDirection = CharacterRotation.Vector(); 
			LaunchDirection.Normalize(); // vector normallize
			float LaunchStrength = 30.0f; 
			LaunchDirection.Z = 25.0f;

			AnimInstance->Montage_Play(DoubleJumpMontage);
			AnimInstance->Montage_JumpToSection(FName("DoubleJump"));

			LaunchCharacter(LaunchDirection * LaunchStrength, false, false);
		}

	}
}


void AVirusCharacter::Scan(const FInputActionValue& Value)
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	//if (!bisDoubleJump)
	//{
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

		const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
		if (BarrelSocket)
		{
			const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

			if (LaserFlash)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LaserFlash, SocketTransform.GetLocation());
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
					if (HitProgram)
					{
						//Head Shot
						if (BeamHitResult.BoneName.ToString() == HitProgram->GetHeadBone())
						{
							UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), 50.f, GetController(), this, UDamageType::StaticClass());
						}
						//Body Shot
						else
						{
							UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), 20.f, GetController(), this, UDamageType::StaticClass());
						}
						
						UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *BeamHitResult.BoneName.ToString());
					}
					else
					{
						//Spawn default particles
						if (ImpactParticles)
						{
							UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactParticles, BeamHitResult.Location);
						}
					}
				}
				
			}

			if (BeamParticles)
			{
				UNiagaraComponent* Beam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BeamParticles, SocketTransform.GetLocation());
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
				}
			}
		//}
	}
	
}



bool AVirusCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector2D ViewportSize;
	FVector OutBeamLocation;

	//GEngine -> global engine pointer: hold viewport
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//Get Screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition; 
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld) //was deprojection successful?
	{

		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		//set beam end point to line trace end point
		OutBeamLocation = End;

		//Trace outward from crosshairs world location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);
		if (ScreenTraceHit.bBlockingHit) //was there a trace hit?
		{
			//Beam end point is now trace hit location
			OutBeamLocation = ScreenTraceHit.Location;
			//perform a second trace, this time fro, the gun barrel
			const FVector WeaponTraceStart{ MuzzleSocketLocation };
			const FVector StartToEnd{ OutBeamLocation - WeaponTraceStart };
			const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f};
			GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
			
			if (!OutHitResult.bBlockingHit) // object between barrel and BeamEndpoint?
			{
				OutHitResult.Location = OutBeamLocation;
				//DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponTraceEnd, FColor::Red, false, 2.f);
				
			}
			return true;
		}

	}
	return false;
}

void AVirusCharacter::Heal(const FInputActionValue& Value)
{

	const USkeletalMeshSocket* HealVFXSocket = GetMesh()->GetSocketByName("HealVFX");
	if (HealVFXSocket)
	{
		const FTransform HealVFXSocketTransform = HealVFXSocket->GetSocketTransform(GetMesh());

		if (HealingVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HealingVFX, HealVFXSocketTransform);
		}

	}
}

void AVirusCharacter::Aiming(const FInputActionValue& Value)
{
	bAiming = true;
}


void AVirusCharacter::StopAiming(const FInputActionValue& Value)
{
	bAiming = false;
}

void AVirusCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
		CameraBoom->TargetArmLength = 150.0f;
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
		CameraBoom->TargetArmLength = 200.0f;
	}
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

void AVirusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle interpolation for zoom when aiming
	CameraInterpZoom(DeltaTime);
	//Calculate Crosshair Spread Multiplier
	CalculateCrosshairSpread(DeltaTime);
}

float AVirusCharacter::GetCrosshairSpreadMultiplier() const
{

	return CrosshairSpreadMultiplier;
}
