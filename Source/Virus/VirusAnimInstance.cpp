// Fill out your copyright notice in the Description page of Project Settings.


#include "VirusAnimInstance.h"
#include "VirusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UVirusAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	//�� �����Ӹ��� ĳ���͸� �����ϰ� �ִ��� Ȯ���ϰ� ���� ���, Casting ����.
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
		//isfalling �Լ��� �߶��� ���� �ο� ���� ��ȯ�Ѵ�.
		bIsInAir = MainCharacter->GetCharacterMovement()->IsFalling();

		//is the character is Jumping??
		if (Velocity.Z > 10.0f && bIsInAir)
		{
			bIsJumping = true;
		}
		else bIsJumping = false;


		//is the character accelerating??
		//GetCurrentAcceleration().Size() : ���� ������ ũ�⸦ ���ϴ� �Լ� -> 0���� ũ�� �����Ѵٰ� ����
		if (MainCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else bIsAccelerating = false;

	}
}

//�ʱ�ȭ �۾�
void UVirusAnimInstance::NativeInitializeAnimation()
{

	//Pawn�� AnumInstance�� �������� �Լ�
	//TryGetPawnOwner �� �Լ��� Pawn�� �����Ѵ�. 
	//AVirusCharacter �����Ϳ� Pawn ĳ���Ͱ� �����ȴ�. 
	MainCharacter = Cast<AVirusCharacter>(TryGetPawnOwner());
}
