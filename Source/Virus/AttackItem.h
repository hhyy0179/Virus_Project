// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackItem.generated.h"


UENUM(BlueprintType)
enum class EAttackItemStatus : uint8
{
	EAIS_Falling UMETA(DisplayName = "Falling"),
	EAIS_Place UMETA(DisplayName = "Place"),
	EAIS_Finish UMETA(DisplayName = "Finish"),

	EAIS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class VIRUS_API AAttackItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAttackItem();

	// Called every frame
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

	void SetItemProperties(EAttackItemStatus State);


private:
	FTimerHandle ThrowItemTimer;
	float ThrowItemTime;
	bool bFalling;
	bool bOverlapped;

	/** Skeletal Mesh for the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemMesh;

	/** Enables item tracing when overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* AttackRange;

	/** State of the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	EAttackItemStatus ItemStatus;

public:

	/** Adds the impulse to the Weapon */
	void ThrowItem();

	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }

	FORCEINLINE EAttackItemStatus GetHealStatus() const { return ItemStatus; }
	void SetAttackItemStatus(EAttackItemStatus Status);
	FORCEINLINE bool GetItemOverlap() const { return bOverlapped; }


};
