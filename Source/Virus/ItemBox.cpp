// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "VirusCharacter.h"

// Sets default values
AItemBox::AItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxBottom = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BoxBottom"));
	SetRootComponent(BoxBottom);

	BoxTop = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BoxTop"));
	BoxTop->SetupAttachment(GetRootComponent());

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());

	//Collision Control Only LineTrace
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickUpWidget->SetupAttachment(GetRootComponent());

	WarningWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WarningWidget"));
	WarningWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItemBox::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetBoxTopMesh()->GetAnimInstance();
	//Hide Pickup Widget
	PickUpWidget->SetVisibility(false);
	WarningWidget->SetVisibility(false);
}


void AItemBox::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AVirusCharacter* Virus = Cast<AVirusCharacter>(OtherActor);
		if (Virus)
		{
			Virus->IncrementOverlappedItemCount(1);
		}
	}
}


void AItemBox::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AVirusCharacter* Virus = Cast<AVirusCharacter>(OtherActor);
		if (Virus)
		{
			Virus->IncrementOverlappedItemCount(-1);
		}
	}
}

// Called every frame
void AItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBox::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItemBox::EnableCustomDepth()
{
	BoxTop->SetRenderCustomDepth(true);
	BoxBottom->SetRenderCustomDepth(true);

}

void AItemBox::DisableCustomDepth()
{
	BoxTop->SetRenderCustomDepth(false);
	BoxBottom->SetRenderCustomDepth(false);
}

void AItemBox::PlayAnimMontage()
{
	if (AnimInstance && OpenMontage)
	{
		AnimInstance->Montage_Play(OpenMontage);
		if (OpenVFX)
		{
			const FTransform BoxTransform = BoxBottom->GetRelativeTransform();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OpenVFX, BoxTransform);
		}
	}

}



