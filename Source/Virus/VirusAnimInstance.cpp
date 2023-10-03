// Fill out your copyright notice in the Description page of Project Settings.


#include "VirusAnimInstance.h"
#include "VirusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UVirusAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	//매 프레임마다 캐릭터를 참조하고 있는지 확인하고 없을 경우, Casting 해줌.
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
		//isfalling 함수는 추락에 대한 부울 값을 반환한다.
		bIsInAir = MainCharacter->GetCharacterMovement()->IsFalling();

		//is the character is Jumping??
		if (Velocity.Z > 10.0f && bIsInAir)
		{
			bIsJumping = true;
		}
		else bIsJumping = false;


		//is the character accelerating??
		//GetCurrentAcceleration().Size() : 가속 벡터의 크기를 구하는 함수 -> 0보다 크면 가속한다고 가정
		if (MainCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else bIsAccelerating = false;

	}
}

//초기화 작업
void UVirusAnimInstance::NativeInitializeAnimation()
{

	//Pawn의 AnumInstance를 가져오는 함수
	//TryGetPawnOwner 이 함수는 Pawn을 리턴한다. 
	//AVirusCharacter 포인터에 Pawn 캐릭터가 참조된다. 
	MainCharacter = Cast<AVirusCharacter>(TryGetPawnOwner());
}
