// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIOperatingSystem.generated.h"

UCLASS()
class VIRUS_API AAIOperatingSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIOperatingSystem();
	void Die();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float Health = 1000.f;
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float MaxHealth = 1000.f;
	class AHackingGaugeManager* GaugeManager;


};
