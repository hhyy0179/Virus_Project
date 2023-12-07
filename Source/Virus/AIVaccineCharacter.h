// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBaseCharacter.h"
#include "AIVaccineCharacter.generated.h"

/**
 * 
 */
UCLASS()
class VIRUS_API AAIVaccineCharacter : public AAIBaseCharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AAIVaccineCharacter();
	void CountHP(int Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Die();
};
