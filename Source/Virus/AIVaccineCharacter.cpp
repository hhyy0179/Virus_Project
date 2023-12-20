// Fill out your copyright notice in the Description page of Project Settings.


#include "AIVaccineCharacter.h"
#include "AIAllyCharacter.h"

AAIVaccineCharacter::AAIVaccineCharacter()
{
	MaxHP = 50;
}

void AAIVaccineCharacter::BeginPlay()
{
}

void AAIVaccineCharacter::CountHP(int Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Count"));
	CurrentHP -= Value;

	if (CurrentHP <= 0) {
		Die();
	}
}



void AAIVaccineCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Die!"));

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
