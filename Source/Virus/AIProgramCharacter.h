// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Components/WidgetComponent.h"
#include "HPBarWidget.h"
#include "AIProgramCharacter.generated.h"

/**
 * 
 */
UCLASS()
class VIRUS_API AAIProgramCharacter : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	AAIProgramCharacter();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ShowHealthBar();

	void ShowHealthBar_Implementation();

	void CloneActor();

	UFUNCTION()
	void HideHealthBar();

	class AHackingGaugeManager* GaugeManager;

public:
	/** Particles to spawn when hit by bullets */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactParicles;

	/** Sound to play when hit by bullets */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Spawn, meta = (AllowPrivateAccess = "true"))
	class UBlueprint* SpawnAllyBP;

	UWidgetComponent* GetWidgetComponentFromActor();

	UWidgetComponent* HPBarWidgetComponent = nullptr;
	UHPBarWidget* HPBar = nullptr;

	UAnimSequence* DieAnim;

	float DieAnimTime = 2.75f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool StopRoaming;

	/** Current health of the enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	/** Max health of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FString HeadBone;

	/** Time to display Health Bar once shot */
	FTimerHandle Timer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;

	void Die();

	bool isDying = false;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(FHitResult HitResult) override;
	
	void BulletHit(FHitResult HitResult, float Damage);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float DamageAmount);

	FORCEINLINE FString GetHeadBone() const { return HeadBone; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentHP() { return Health / MaxHealth; }
};