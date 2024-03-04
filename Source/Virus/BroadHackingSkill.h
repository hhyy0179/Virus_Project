// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "BroadHackingSkill.generated.h"


UENUM(BlueprintType)
enum class EBroadHackingSkillStatus : uint8
{
	EBHSS_Falling UMETA(DisplayName = "Falling"),
	EBHSS_BroadAttack UMETA(DisplayName = "BroadAttack"),
	EBHSS_Finish UMETA(DisplayName = "Finish"),

	EBHSS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class VIRUS_API ABroadHackingSkill : public AActor
{
	GENERATED_BODY()

public:
	ABroadHackingSkill();

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

	void SkillAction();

	void StopAttack();

	void SetBroadHackingProperties(EBroadHackingSkillStatus State);


private:
	FTimerHandle ThrowItemTimer;
	float ThrowItemTime;
	bool bFalling;

	FTimerHandle AttackTimer;
	float AttackTime;
	bool bAttackOverlapped;


	/** Skeletal Mesh for the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/** Enables item tracing when overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* AttackRange;

	/** State of the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	EBroadHackingSkillStatus AttackStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VFX, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* EnemyHitVFX;

public:

	/** Adds the impulse to the Weapon */
	void ThrowItem(float CharacterSpeed);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	FORCEINLINE EBroadHackingSkillStatus GetAttackStatus() const { return AttackStatus; }
	void SetBroadHackingStatus(EBroadHackingSkillStatus Status);
	FORCEINLINE bool GetAttackOverlap() const { return bAttackOverlapped; }

};
