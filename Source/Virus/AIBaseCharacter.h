// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIBaseCharacter.generated.h"

UCLASS()
class VIRUS_API AAIBaseCharacter : public ACharacter
{
	GENERATED_BODY()



public:
	// Sets default values for this character's properties
	AAIBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Die();

	void HPCalculate(int Value);
	void SetMaxHP(int Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int MaxHP = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentHP = 0;
};
