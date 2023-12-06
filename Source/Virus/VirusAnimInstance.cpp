// Fill out your copyright notice in the Description page of Project Settings.


#include "VirusAnimInstance.h"
#include "VirusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"



void UVirusAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (MainCharacter == nullptr)
	{
		MainCharacter = Cast<AVirusCharacter>(TryGetPawnOwner());
	}
	if (MainCharacter)
	{
		//Get the speed of the character from velocity
		FVector Velocity{ MainCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//is the character in the air??
		//isfalling
		bIsInAir = MainCharacter->GetCharacterMovement()->IsFalling();

		//is the character is Jumping??
		if (Velocity.Z > 10.0f && bIsInAir)
		{
			bIsJumping = true;
		}
		else bIsJumping = false;


		//is the character accelerating??
		//GetCurrentAcceleration().Size() 
		if (MainCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else bIsAccelerating = false;

		FRotator AimRotation = MainCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MainCharacter->GetVelocity());
		MovementOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		/*FString OffsetMessage = FString::Printf(TEXT("Movement Offset Yaw: %f"), MovementOffset);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, OffsetMessage);
		}*/

		if (MainCharacter->GetVelocity().Size() > 0.f)
		{
			
		}
		bAiming = MainCharacter->GetAiming();
		bDoubleJump = MainCharacter->GetDoubleJump();
	}
}

void UVirusAnimInstance::NativeInitializeAnimation()
{

	MainCharacter = Cast<AVirusCharacter>(TryGetPawnOwner());
}
