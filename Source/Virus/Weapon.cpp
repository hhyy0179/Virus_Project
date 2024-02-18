// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "VirusCharacter.h"

AWeapon::AWeapon() :
	ThrowWeaponTime(0.7f),
	bFalling(false),
	GageAmount(1.0f),
	GageDrainRate(0.1f),
	MaxGageAmount(1.0f),
	GageStatus(EWeapongageStatus::EWS_Normal)
{
	PrimaryActorTick.bCanEverTick = true;
	GageAmount = MaxGageAmount;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Keep the weapon upright
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const  FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (Character)
	{
		SetWeapongageProperties(GageStatus, DeltaTime);
	}

}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorScale3D(FVector(1.0f));
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };

	//Direction in which we throw the weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation = { 30.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 1'000.f;

	//Use MeshRight to randomize
	GetItemMesh()->AddImpulse(ImpulseDirection);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, GetItemMesh()->GetName());
	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	GageAmount = MaxGageAmount;
	GageStatus = EWeapongageStatus::EWS_Normal;
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_Pickup);
}

void AWeapon::SetWeapongageProperties(EWeapongageStatus State, float DeltaTime)
{
	float DeltaGage = GageDrainRate * DeltaTime;

	switch (State)
	{
	case EWeapongageStatus::EWS_Normal:

		if (Character->GetScanning())
		{
			GageAmount -= DeltaGage * 2;

			if (GageAmount <= 0.f)
			{
				SetWeapongageStatus(EWeapongageStatus::EWS_Empty);
			}
		}
		break;

	case EWeapongageStatus::EWS_Empty:
		GageAmount = 0.f;
		SetWeapongageStatus(EWeapongageStatus::EWS_Reloading);
		break;

	case EWeapongageStatus::EWS_Reloading:

		if (GageAmount <= MaxGageAmount)
		{
			GageAmount += DeltaGage * 5;
		}
		else
		{
			GageAmount = MaxGageAmount;
			SetWeapongageStatus(EWeapongageStatus::EWS_Normal);
		}
		break;
	}
}
