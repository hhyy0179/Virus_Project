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
#include "HPBarWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "AIAllyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HackingGaugeManager.h"
#include "Misc/OutputDeviceNull.h"

AAIProgramCharacter::AAIProgramCharacter() :
	MaxHealth(50.0f),
	HeadBone("Head"),
	HealthBarDisplayTime(4.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = MaxHealth;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> anim(TEXT("/Game/_VirusGame/AI/AIProgramCharacter/Anim_AIProgram/NewWalking/Mutant_Dying.Mutant_Dying"));

	if (anim.Succeeded()) {
		DieAnim = anim.Object;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("This is Null Animation"));
	}


	/*HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));

	static ConstructorHelpers::FClassFinder<UHPBarWidget> HealthBarClass(TEXT("/Game/_VirusGame/HUD/BP_NewAIHPBar.BP_NewAIHPBar_C"));
	if (HealthBarClass.Succeeded())
	{
		HealthBarWidget->SetWidgetClass(HealthBarClass.Class);

		UE_LOG(LogTemp, Warning, TEXT("There is HealthBarclass"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no HealthBarClass"));
	}*/
}

void AAIProgramCharacter::BeginPlay() {
	Super::BeginPlay();


	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHackingGaugeManager::StaticClass(), FoundActors);
	GaugeManager = Cast<AHackingGaugeManager>(FoundActors[0]);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	Health = MaxHealth;
	GetWidgetComponentFromActor();
	HideHealthBar();

	StopRoaming = false;
}

void AAIProgramCharacter::ShowHealthBar()
{
	HPBarWidgetComponent->SetVisibility(true);
	HPBar->SetPercent(Health / MaxHealth);
	GetWorldTimerManager().ClearTimer(Timer);
	GetWorldTimerManager().SetTimer(Timer, this, &AAIProgramCharacter::HideHealthBar, HealthBarDisplayTime);
}

void AAIProgramCharacter::HideHealthBar()
{
	HPBarWidgetComponent->SetVisibility(false);
}

UWidgetComponent* AAIProgramCharacter::GetWidgetComponentFromActor()
{
	if (this)
	{
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

void AAIProgramCharacter::ShowHealthBar_Implementation()
{

}

void AAIProgramCharacter::Die()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Program Die"));

	StopRoaming = true;
	isDying = true;

	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	GetMesh()->PlayAnimation(DieAnim, false);

	UE_LOG(LogTemp, Warning, TEXT("CloneActor"));

	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	GetMesh()->PlayAnimation(DieAnim, false);

	HideHealthBar();
	GetWorldTimerManager().ClearTimer(Timer);
	GetWorldTimerManager().SetTimer(Timer, this, &AAIProgramCharacter::CloneActor, DieAnimTime);

	GaugeManager->Percent += 0.01f;
	GaugeManager->ControlGauge(GaugeManager->Percent);
}

void AAIProgramCharacter::CloneActor()
{
	const FString command = FString::Printf(TEXT("SpawnAlly"));
	FOutputDeviceNull Ar;

	CallFunctionByNameWithArguments(*command, Ar, NULL, true);
	/*GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Call Clone Actor"));
	FTransform ActorTransform = this->GetActorTransform();
	this->SetActorLocation(FVector(0.f, 0.f, 0.f));

	if (SpawnAllyBP)
	{
		GetWorld()->SpawnActor<AAIAllyCharacter>(SpawnAllyBP->GeneratedClass, ActorTransform);

		GetWorld()->DestroyActor(this);
	}
	else
	{
		if (!SpawnAllyBP) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SpawnAllyBP is not exist"));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("I Don't Know Reason"));

		}
	}*/
}

void AAIProgramCharacter::Tick(float DeltaTime)
{

}

void AAIProgramCharacter::BulletHit_Implementation(FHitResult HitResult)
{

}

void AAIProgramCharacter::BulletHit(FHitResult HitResult, float Damage)
{
	ShowHealthBar();

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

float AAIProgramCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Current Health: %f"), Health);

	ShowHealthBar();

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

void AAIProgramCharacter::TakeDamage(float DamageAmount)
{
	Health -= DamageAmount;

	UE_LOG(LogTemp, Warning, TEXT("Current Health: %f"), Health);

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	ShowHealthBar();

	if (Health <= 0) {
		Die();
	}
}


