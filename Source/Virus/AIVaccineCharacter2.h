// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "HPBarWidget.h"
#include "Components/WidgetComponent.h"
#include "AIVaccineCharacter2.generated.h"

UCLASS()
class VIRUS_API AAIVaccineCharacter2 : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIVaccineCharacter2();
protected:
	virtual void BeginPlay() override;

	UWidgetComponent* GetWidgetComponentFromActor();

	UFUNCTION()
	void ShowHealthBar();

	UFUNCTION()
	void HideHealthBar();

	class AHackingGaugeManager* GaugeManager;

	void SpawnKey();

	void Die();
public:
	/** Current health of the enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Health;

	/** Max health of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HeadBone;

	/** Time to display Health Bar once shot */
	FTimerHandle HealthBarTimer;

private:

	/** Particles to spawn when hit by bullets */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactParicles;

	/** Sound to play when hit by bullets */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AItem> KeyClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess = "true"))
	AItem* SpawnedKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess = "true"))
	bool bFalling;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	void TakeDamage(float DamageAmount);

	virtual void BulletHit_Implementation(FHitResult HitResult) override;

	void BulletHit(FHitResult HitResult, float Damage);

	FORCEINLINE FString GetHeadBone() const { return HeadBone; }

	UFUNCTION(BlueprintCallable)
		float GetCurrentHP() { return Health / MaxHealth; }

	UWidgetComponent* HPBarWidgetComponent = nullptr;
	UHPBarWidget* HPBar = nullptr;

};
