// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"

#include "Engine/World.h"
#include "Components/WidgetComponent.h"
#include "HPBarWidget.h"

// Sets default values
AAIBaseCharacter::AAIBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHP = MaxHP;
}

// Called every frame
void AAIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AAIBaseCharacter::SetMaxHP(int Value)
{
	MaxHP = Value;
}

void AAIBaseCharacter::CountHP(int Value)
{
	CurrentHP -= Value;

	UE_LOG(LogTemp, Warning, TEXT("Parent HP %d"), CurrentHP);

	if (CurrentHP <= 0) {
		GetWorld()->DestroyActor(this);
	}
}

float AAIBaseCharacter::GetHP()
{
	return CurrentHP / MaxHP;
}

