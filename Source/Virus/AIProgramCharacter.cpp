// Fill out your copyright notice in the Description page of Project Settings.


#include "AIProgramCharacter.h"
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
#include "AIAllyCharacter.h"

AAIProgramCharacter::AAIProgramCharacter():
	MaxHealth(100.f),
	HeadBone("Head"),
	HealthBarDisplayTime(4.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}



void AAIProgramCharacter::BeginPlay() {
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	Health = MaxHealth;
	GetWidgetComponentFromActor();
	HideHealthBar();
}

UWidgetComponent* AAIProgramCharacter::GetWidgetComponentFromActor()
{
	if (this)
	{
		// 액터의 루트 컴포넌트를 가져옵니다.

		if (RootComponent)
		{
			UWidgetComponent* WidgetComponent = nullptr;

			for (USceneComponent* ChildComponent : RootComponent->GetAttachChildren())
			{
				if (ChildComponent->IsA<UWidgetComponent>())
				{
					WidgetComponent = Cast<UWidgetComponent>(ChildComponent);
					break; 
				}
			}

			if (WidgetComponent)
			{
				HPBarWidget = WidgetComponent;
				UE_LOG(LogTemp, Warning, TEXT("I Find WidgetComponent"));
				return WidgetComponent;
			}
			else 
			{
				UE_LOG(LogTemp, Warning, TEXT("I Can't Find WidgetComponent"));
			}
		}
	}
	return nullptr;
}

void AAIProgramCharacter::ShowHealthBar()
{
	HPBarWidget->SetVisibility(true);
}

void AAIProgramCharacter::HideHealthBar()
{
	UE_LOG(LogTemp, Warning, TEXT("Hide Health Bar@@@@@@@"));
	HPBarWidget->SetVisibility(false);
}

void AAIProgramCharacter::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AAIProgramCharacter::HideHealthBar, HealthBarDisplayTime);
}

void AAIProgramCharacter::Die()
{
	HideHealthBar();
	CloneActor();
	GetWorld()->DestroyActor(this);
}

void AAIProgramCharacter::CloneActor()
{
	TSubclassOf<AAIAllyCharacter> NewActorClass = AAIAllyCharacter::StaticClass();
	UObject* ClassPackage = ANY_PACKAGE;

	UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/_VirusGame/AI/BP_AIAllyCharacter.BP_AIAllyCharacter")));
	UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);

	if (this && SpawnActor)
	{
		FTransform ActorTransform = this->GetActorTransform();
		this->SetActorLocation(FVector(0.f, 0.f, 0.f));

		// 새로운 액터 생성
		AAIAllyCharacter* NewActor = GetWorld()->SpawnActor<AAIAllyCharacter>(GeneratedBP->GeneratedClass, ActorTransform);
		UE_LOG(LogTemp, Warning, TEXT("CloneActor Success"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnToActor Class is None"));
	}
}

void AAIProgramCharacter::Tick(float DeltaTime)
{

}

void AAIProgramCharacter::BulletHit_Implementation(FHitResult HitResult)
{
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

float AAIProgramCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Die();
		Health = 0.f;
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}


