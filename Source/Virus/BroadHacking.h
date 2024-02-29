// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BroadHacking.generated.h"


UENUM(BlueprintType)
enum class EBroadHackingStatus : uint8
{
	EBHS_Hacking UMETA(DisplayName = "Hacking"),
	EBHS_Finish UMETA(DisplayName = "Finish"),

	EBHS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class VIRUS_API ABroadHacking : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABroadHacking();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Callback function for AddDynamic
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void StopHacking();

	void SetBroadHackingProperties(EBroadHackingStatus State);

private:

	FTimerHandle HackingTimer;
	float HackingTime;
	bool bAttackOverlapped;

	FTimerHandle SkillCoolTimer;
	float CoolTime;
	bool bCoolTime;


	/** Enables item tracing when overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* AttackRange;

	/** State of the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	EBroadHackingStatus BroadHackingStatus;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HackingAction();

	FORCEINLINE EBroadHackingStatus GetBroadHackingStatus() const { return BroadHackingStatus; }

	void SetBroadHackingStatus(EBroadHackingStatus Status);

	FORCEINLINE bool GetAttackOverlap() const { return bAttackOverlapped; }

	FORCEINLINE UStaticMeshComponent* GetStaticMesh() const { return AttackRange; }

};
