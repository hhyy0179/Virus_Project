// Fill out your copyright notice in the Description page of Project Settings.


#include "AIVaccineCharacter.h"

AAIVaccineCharacter::AAIVaccineCharacter()
{
	MaxHP = 50;
}

void AAIVaccineCharacter::BeginPlay()
{
}

void AAIVaccineCharacter::CountHP(int Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Child  Count"));
	CurrentHP -= Value;

	if (CurrentHP <= 0) {
		Die();
	}
}

void AAIVaccineCharacter::Die()
{
	GetWorld()->DestroyActor(this);
}
