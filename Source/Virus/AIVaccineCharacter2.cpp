// Fill out your copyright notice in the Description page of Project Settings.


#include "AIVaccineCharacter2.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "HackingGaugeManager.h"
#include "Engine/World.h"
#include "AIAllyCharacter.h"

// Sets default values
AAIVaccineCharacter2::AAIVaccineCharacter2():
	MaxHealth(100.f),
	HeadBone("Head"),
	HealthBarDisplayTime(4.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void AAIVaccineCharacter2::BeginPlay() {
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHackingGaugeManager::StaticClass(), FoundActors);
	GaugeManager = Cast<AHackingGaugeManager>(FoundActors[0]);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	Health = MaxHealth;
	GetWidgetComponentFromActor();
	HideHealthBar();
}

UWidgetComponent* AAIVaccineCharacter2::GetWidgetComponentFromActor()
{
	if (this)
	{
		// 액터의 루트 컴포넌트를 가져옵니다.

		if (RootComponent)
		{
			for (USceneComponent* ChildComponent : RootComponent->GetAttachChildren())
			{
				if (ChildComponent->IsA<UWidgetComponent>())
				{
					HPBarWidgetComponent = Cast<UWidgetComponent>(ChildComponent);

					if (HPBarWidgetComponent)
					{
						HPBar = Cast<UHPBarWidget>(HPBarWidgetComponent->GetUserWidgetObject());

						if (HPBar) {
						}
						else {
							UE_LOG(LogTemp, Warning, TEXT("Get HP Fail"));
						}

						return HPBarWidgetComponent;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("I Can't Find WidgetComponent"));
					}

					break;
				}
			}

		}
	}
	return nullptr;
}

void AAIVaccineCharacter2::ShowHealthBar()
{
	HPBar->SetPercent(Health / MaxHealth);
	HPBarWidgetComponent->SetVisibility(true);
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AAIVaccineCharacter2::HideHealthBar, HealthBarDisplayTime);
}

void AAIVaccineCharacter2::HideHealthBar()
{
	HPBarWidgetComponent->SetVisibility(false);
}

void AAIVaccineCharacter2::Die()
{
	GaugeManager->ControlGauge(0.01f);
	HideHealthBar();
	GetWorld()->DestroyActor(this);
	GaugeManager->ControlGauge(0.01f);
	FTransform ActorTransform = this->GetActorTransform();
	this->SetActorLocation(FVector(0.f, 0.f, 0.f));
	
	int RandomValue = FMath::RandRange(1, 10);
	if (RandomValue < 8) {
		// Spawn Actor
		TSubclassOf<AAIAllyCharacter> NewActorClass = AAIAllyCharacter::StaticClass();
		UObject* ClassPackage = ANY_PACKAGE;

		UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/_VirusGame/AI/AIAllyCharacter/BP_AIAllyCharacter.BP_AIAllyCharacter")));
		UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);
		GetWorld()->SpawnActor<AAIAllyCharacter>(GeneratedBP->GeneratedClass, ActorTransform);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Fail RandomValue: %d"), RandomValue);
	}
}

void AAIVaccineCharacter2::Tick(float DeltaTime)
{

}

float AAIVaccineCharacter2::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Take Damage"));

	ShowHealthBar();

	if (Health - DamageAmount == 0.f)
	{
		Die();
		Health = 0.f;
	}
	else if (Health - DamageAmount > 0.f)
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AAIVaccineCharacter2::BulletHit_Implementation(FHitResult HitResult)
{
}

void AAIVaccineCharacter2::BulletHit(FHitResult HitResult, float Damage)
{
	Health -= Damage;

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());


	}
	if (ImpactParicles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParicles, HitResult.Location, FRotator(0.f), true);
	}

	ShowHealthBar();
}


