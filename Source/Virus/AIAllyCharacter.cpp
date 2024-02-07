// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAllyCharacter.h"
#include "AIProgramCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

void AAIAllyCharacter::CloneActor()
{
	TSubclassOf<AAIProgramCharacter> NewActorClass = AAIProgramCharacter::StaticClass();
	UObject* ClassPackage = ANY_PACKAGE;

	UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/_VirusGame/AI/AIProgramCharacter/BP_AIProgramCharacter.BP_AIProgramCharacter")));
	UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);

	if (this && SpawnActor)
	{
		FTransform ActorTransform = this->GetActorTransform();
		this->SetActorLocation(FVector(0.f, 0.f, 0.f));

		GetWorld()->DestroyActor(this);

		AAIProgramCharacter* NewActor = GetWorld()->SpawnActor<AAIProgramCharacter>(GeneratedBP->GeneratedClass, ActorTransform);
		NewActor->GetCharacterMovement()->MaxWalkSpeed = 150.f;
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnToActor Class is None"));
	}
}

