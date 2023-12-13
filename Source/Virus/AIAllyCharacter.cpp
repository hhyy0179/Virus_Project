// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAllyCharacter.h"

// Sets default values
AAIAllyCharacter::AAIAllyCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIAllyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIAllyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIAllyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

