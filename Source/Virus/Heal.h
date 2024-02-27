// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Heal.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EHealStatus : uint8
{
	EHS_Falling UMETA(DisplayName = "Falling"),
	EHS_Healing UMETA(DisplayName = "Healing"),
	EHS_Finish UMETA(DisplayName = "Finish"),

	EHS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class VIRUS_API AHeal : public AActor
{
	GENERATED_BODY()
	


public:
	AHeal();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Callback function for AddDynamic
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void StopFalling();

	void HealAction();

	void StopHealing();

	void SetHealProperties(EHealStatus State);
	
private:
	FTimerHandle ThrowHealPackTimer;
	float ThrowHealPackTime;
	bool bFalling;

	FTimerHandle HealingTimer;
	float HealingTime;
	bool bHealOverlapped;


	/** Skeletal Mesh for the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/** Enables item tracing when overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* HealRange;

	/** State of the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	EHealStatus HealStatus;

public:

	/** Adds the impulse to the Weapon */
	void ThrowHealPack();

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	
	FORCEINLINE EHealStatus GetHealStatus() const { return HealStatus; }
	void SetHealStatus(EHealStatus Status);
	FORCEINLINE bool GetHealOverlap() const { return bHealOverlapped; }

	
};
