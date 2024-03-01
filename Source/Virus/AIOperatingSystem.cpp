// Fill out your copyright notice in the Description page of Project Settings.


#include "AIOperatingSystem.h"
#include "HackingGaugeManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAIOperatingSystem::AAIOperatingSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIOperatingSystem::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHackingGaugeManager::StaticClass(), FoundActors);
	GaugeManager = Cast<AHackingGaugeManager>(FoundActors[0]);
}

void AAIOperatingSystem::Die()
{
	
		
	GaugeManager->Percent = 100.f;
	GaugeManager->ControlGauge(GaugeManager->Percent);
	
	GetWorld()->DestroyActor(this);
}

// Called every frame
void AAIOperatingSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

