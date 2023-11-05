// Copyright Epic Games, Inc. All Rights Reserved.

#include "VirusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "AIBaseCharacter.h"

#include "DrawDebugHelpers.h"

#include "AIBaseCharacter.h"


//////////////////////////////////////////////////////////////////////////
// AVirusCharacter

AVirusCharacter::AVirusCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

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
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	/*static ConstructorHelpers::FObjectFinder<UInputAction>IA_Jump(
		TEXT("/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump"));
	if (IA_Jump.Succeeded())
	{
		JumpAction = IA_Jump.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Look(
		TEXT("/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look"));
	if (IA_Look.Succeeded())
	{
		LookAction = IA_Look.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Move(
		TEXT("/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move"));
	if (IA_Move.Succeeded())
	{
		MoveAction = IA_Move.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Scan(
		TEXT("/Game/ThirdPerson/Input/Actions/IA_Scan.IA_Scan"));
	if (IA_Scan.Succeeded())
	{
		ScanAction = IA_Scan.Object;
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
}

//////////////////////////////////////////////////////////////////////////
// Input

void AVirusCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AVirusCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVirusCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVirusCharacter::Look);

		//Scanning
		EnhancedInputComponent->BindAction(ScanAction, ETriggerEvent::Started, this, &AVirusCharacter::Scan);

		//Scanning
		EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Started, this, &AVirusCharacter::Heal);
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

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AVirusCharacter::Jump(const FInputActionValue& Value)
{
	
	if (JumpCurrentCount)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && DoubleJumpMontage && bisDoubleJump)
		{
			FRotator CharacterRotation = GetActorRotation(); // 캐릭터의 현재 회전 방향 가져오기
			FVector LaunchDirection = CharacterRotation.Vector(); // 회전 방향을 벡터로 변환
			LaunchDirection.Normalize(); // vector normallize
			float LaunchStrength = 60.0f; // 힘의 크기 설정
			LaunchDirection.Z = 10.0f;

			AnimInstance->Montage_Play(DoubleJumpMontage);
			AnimInstance->Montage_JumpToSection(FName("DoubleJump"));

			LaunchCharacter(LaunchDirection * LaunchStrength, false, false);
			bisDoubleJump = false;
		}
	}
	else
	{
		ACharacter::Jump();
		bisDoubleJump = true;
	}
}


void AVirusCharacter::Scan(const FInputActionValue& Value)
{
	if (Controller != nullptr && FireSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack"));
		
		UGameplayStatics::PlaySound2D(this, FireSound);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
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
		

		FVector2D ViewportSize;
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
			FVector BeamEndPoint{ End };

			//Trace outward from crosshairs world location
			GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);

			if (ScreenTraceHit.bBlockingHit) //was there a trace hit?
			{
				//Beam end point is now trace hit location
				BeamEndPoint = ScreenTraceHit.Location;

				if (LaserFlash)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LaserFlash, ScreenTraceHit.Location);
				}

				DrawDebugLine(GetWorld(), Start, BeamEndPoint, FColor::Red, false, 2.f);

				if (Cast<AAIBaseCharacter>(ScreenTraceHit.GetActor())) {
					AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(ScreenTraceHit.GetActor());
					AI->HPCalculate(-10);

					float AIHPPercent = AI->CurrentHP / AI->MaxHP;

					UE_LOG(LogTemp, Warning, TEXT("AI HP: %f"), AIHPPercent);
				}
			}
		}

		/*FHitResult ScanHit;
		const FVector Start{ SocketTransform.GetLocation() };
		const FQuat Rotation{ SocketTransform.GetRotation() };
		const FVector RotationAxis{ Rotation.GetAxisX() };
		const FVector End{ Start + RotationAxis* 50'000.f }; //pointing in the direction of the barrel


		//ScanHit has stored in it regarding the LineTrace information
		//if it hit something, return true, if it didn't return false
		
		if (ScanHit.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);
			//DrawDebugLine(GetWorld(), ScanHit.Location, 5.f, FColor::Red, false, 2.f);

			
		}*/
	}
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


