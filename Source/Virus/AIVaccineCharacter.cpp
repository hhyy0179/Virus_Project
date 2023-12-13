// Fill out your copyright notice in the Description page of Project Settings.


#include "AIVaccineCharacter.h"
#include "AIAllyCharacter.h"

AAIVaccineCharacter::AAIVaccineCharacter()
{
	MaxHP = 50;
}

void AAIVaccineCharacter::BeginPlay()
{
    Die();
}

void AAIVaccineCharacter::CountHP(int Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Child  Count"));
	CurrentHP -= Value;

	if (CurrentHP <= 0) {
		Die();
	}
}

void AAIVaccineCharacter::CloneActor()
{
    TSubclassOf<AAIAllyCharacter> NewActorClass = AAIAllyCharacter::StaticClass();
    UObject* ClassPackage = ANY_PACKAGE;

    UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/_VirusGame/AI/BP_AIAllyCharacter.BP_AIAllyCharacter")));
    UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);

    if (this && SpawnActor)
    {
        FTransform ActorTransform = this->GetActorTransform();
        this->SetActorLocation(FVector(0.f, 0.f, 0.f));

        // 货肺款 咀磐 积己
        AAIAllyCharacter* NewActor = GetWorld()->SpawnActor<AAIAllyCharacter>(GeneratedBP->GeneratedClass, ActorTransform);
        UE_LOG(LogTemp, Warning, TEXT("CloneActor Success"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnToActor Class is None"));
    }
}

void AAIVaccineCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Die!"));
    CloneActor();
	GetWorld()->DestroyActor(this);
}

void AAIVaccineCharacter::SetMaxHP(float Value)
{
	MaxHP = Value;
}

void AAIVaccineCharacter::CountHP(float Value)
{
	CurrentHP -= Value;
}

float AAIVaccineCharacter::GetHP()
{
	return CurrentHP;
}
