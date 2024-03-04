// Fill out your copyright notice in the Description page of Project Settings.


#include "Heal.h"
#include "Item.h"
#include "VirusCharacter.h"
#include "Components/SphereComponent.h"

AHeal::AHeal():
	ThrowHealPackTime(1.2f),
	bFalling(false),
	HealingTime(5.f),
	bHealOverlapped(false),
	HealStatus(EHealStatus::EHS_Falling)
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

	HealRange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HealRange"));
	HealRange->SetupAttachment(GetRootComponent());

}

void AHeal::BeginPlay()
{
	Super::BeginPlay();

	SetHealProperties(HealStatus);

	//Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AHeal::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AHeal::OnSphereEndOverlap);
}


void AHeal::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AVirusCharacter* Virus = Cast<AVirusCharacter>(OtherActor);
		if (Virus)
		{
			bHealOverlapped = true;
		}
	}
}


void AHeal::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AVirusCharacter* Virus = Cast<AVirusCharacter>(OtherActor);
		if (Virus)
		{
			bHealOverlapped = false;
		}
	}
}

void AHeal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const  FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AHeal::StopFalling()
{
	bFalling = false;
	SetHealStatus(EHealStatus::EHS_Healing);
	HealAction();
}

void AHeal::HealAction()
{
	GetWorldTimerManager().SetTimer(HealingTimer, this, &AHeal::StopHealing, HealingTime);
}

void AHeal::StopHealing()
{
	bHealOverlapped = false;
	SetHealStatus(EHealStatus::EHS_Finish);
}

void AHeal::SetHealProperties(EHealStatus State)
{
	switch (State)  
	{
	case EHealStatus::EHS_Falling:

		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); //모든 채널에 대해서 충돌 제한
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		HealRange->SetVisibility(false);
		HealRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		HealRange->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HealRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		break;

	case EHealStatus::EHS_Healing:

		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		HealRange->SetVisibility(true);
		HealRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		HealRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	
	case EHealStatus::EHS_Finish:

		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		HealRange->SetVisibility(false);
		HealRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		HealRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		break;

	}
}


void AHeal::ThrowHealPack()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorScale3D(FVector(1.0f));
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };

	//Direction in which we throw the HealPack
	/* Legacy */
	//FVector ImpulseDirection = MeshRight.RotateAngleAxis(90.f, MeshForward);

	/* New */
	FVector ImpulseDirection = MeshForward.RotateAngleAxis(20.f, MeshRight);
	
	float RandomRotation = { 30.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 1'000.f;

	//Use MeshRight to randomize
	GetItemMesh()->AddImpulse(ImpulseDirection);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, GetItemMesh()->GetName());
	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowHealPackTimer, this, &AHeal::StopFalling, ThrowHealPackTime);
}

void AHeal::SetHealStatus(EHealStatus Status)
{
	HealStatus = Status;
	SetHealProperties(HealStatus);
}

