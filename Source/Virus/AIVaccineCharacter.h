// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIProgramCharacter.h"
#include "AIVaccineCharacter.generated.h"

/**
 * 
 */
UCLASS()
class VIRUS_API AAIVaccineCharacter : public AAIProgramCharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AAIVaccineCharacter();
	void CountHP(int Value);

	void CloneActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Die();

	void SetMaxHP(float Value);
	void CountHP(float Value);

	UFUNCTION(BlueprintCallable)
	float GetHP();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxHP = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentHP = 0;

};
