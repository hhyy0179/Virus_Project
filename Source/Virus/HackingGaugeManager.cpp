// Fill out your copyright notice in the Description page of Project Settings.


#include "HackingGaugeManager.h"
#include "Blueprint/UserWidget.h"
#include "Misc/OutputDeviceNull.h"
#include "AIAllyCharacter.h"
#include "Kismet/GameplayStatics.h"

FOutputDeviceNull Ar;

// Sets default values
AHackingGaugeManager::AHackingGaugeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<UUserWidget> HackingGauageAsset(TEXT("/Game/_VirusGame/HUD/BP_HackingGauage2.BP_HackingGauage2_C"));

	if (HackingGauageAsset.Succeeded()) {
		HackingGauageClass = HackingGauageAsset.Class;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("there is no HackingGuageWidget"));
	}

}

// Called when the game starts or when spawned
void AHackingGaugeManager::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(HackingGauageClass))
	{
		HackingGauageWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), HackingGauageClass));

		if (IsValid(HackingGauageWidget))
		{
			// À§Á¬À» ºäÆ÷Æ®¿¡ ¶ç¿ì´Â ÇÔ¼ö
			HackingGauageWidget->AddToViewport();

		}
	}
	Percent += 0.1f;
	PreviousPercent += 0.1f;
	ControlGauge(Percent);
}

// Called every frame
void AHackingGaugeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	if (PreviousPercent == Percent) { // There is no Change of Gauge
		if (Percent == FixedPercent[CurrentPercentIndex + 1]) {
			CurrentPercentIndex++; // Current Hacking Gauge is next Fixed rate 
		}
		else if (Percent == FixedPercent[CurrentPercentIndex]) {
		}
		else {
			if (PreviousPercent == Percent) { // There is no Change of Gauge -> Gauge decrease
				Count++;
				if (Count == MaxCount) {
					TArray<AActor*> FoundActors;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIAllyCharacter::StaticClass(), FoundActors);
					if (FoundActors.Num() > 0) {
						AAIAllyCharacter* AIAlly = Cast<AAIAllyCharacter>(FoundActors[0]);
						AIAlly->CloneActor();
						Count = 0;
						Percent -= 0.01f;
						PreviousPercent = Percent;
						ControlGauge(Percent);
					}
					else { // There is no Allies so Gauge can not decrease
						Count = 0;
					}
				}
			}
		}
	}
	else { // There is Change of Gauge
		PreviousPercent = Percent;
		Count = 0;
	}
}

void AHackingGaugeManager::ControlGauge(float Value)
{
	const FString command = FString::Printf(TEXT("UpdatePercent %f"), Value);

	HackingGauageWidget->CallFunctionByNameWithArguments(*command, Ar, NULL, true);
}

