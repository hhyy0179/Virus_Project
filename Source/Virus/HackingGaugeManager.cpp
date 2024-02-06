// Fill out your copyright notice in the Description page of Project Settings.


#include "HackingGaugeManager.h"

// Sets default values
AHackingGaugeManager::AHackingGaugeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHackingGaugeManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHackingGaugeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHackingGaugeManager::ControlGauge(int Value)
{
	Percent += Value;
	UE_LOG(LogTemp, Warning, TEXT("The Guage = %d"), Percent);
}

