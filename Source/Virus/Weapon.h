// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EWeapongageStatus : uint8
{
	EWS_Normal UMETA(DisplayName = "Normal"),
	EWS_Empty UMETA(DisplayName = "Empty"),
	EWS_Reloading UMETA(DisplayName = "Reloading"),

	EWS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class VIRUS_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StopFalling();

	void SetWeapongageProperties(EWeapongageStatus State, float DeltaTime);
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	/** Gage Amount for this Weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float GageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float GageDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float MaxGageAmount;


public:

	/** Adds the impulse to the Weapon */
	void ThrowWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EWeapongageStatus GageStatus;

	FORCEINLINE float GetGageAmount() const { return GageAmount; }
	FORCEINLINE EWeapongageStatus GetWeapongageStatus() const { return GageStatus; }
	FORCEINLINE void SetWeapongageStatus(EWeapongageStatus Status) { GageStatus = Status; }

};
