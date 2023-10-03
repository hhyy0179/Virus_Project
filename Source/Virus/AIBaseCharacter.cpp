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

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetupAttachment(RootComponent);
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

void AAIBaseCharacter::Die()
{
	// Die Anim Play
	// after few seconds or ticks
	GetWorld()->DestroyActor(this);
}

void AAIBaseCharacter::HPCalculate(int Value)
{
	CurrentHP += Value;

	if (CurrentHP == 0) Die();
	else return;
}

void AAIBaseCharacter::SetMaxHP(int Value)
{
	MaxHP = Value;
}

