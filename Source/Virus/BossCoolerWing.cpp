// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCoolerWing.h"

// Sets default values
ABossCoolerWing::ABossCoolerWing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossCoolerWing::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ABossCoolerWing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator RT = GetActorRotation();
	FRotator NewRotation = FRotator(RT.Roll, RT.Pitch + 0.01f, RT.Yaw);
	FQuat NewQuat = NewRotation.Quaternion();
	SetActorRotation(NewQuat);
	UE_LOG(LogTemp, Warning, TEXT("Current Pitch: %f"), GetActorRotation().Pitch);
}

