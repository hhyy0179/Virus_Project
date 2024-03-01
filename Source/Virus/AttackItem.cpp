// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackItem.h"
#include "VirusCharacter.h"
#include "Components/SphereComponent.h"
#include "AIVaccineCharacter2.h"
#include "AIProgramCharacter.h"

// Sets default values
AAttackItem::AAttackItem():
	ThrowItemTime(0.7f),
	bFalling(false),
	bOverlapped(false),
	ItemStatus(EAttackItemStatus::EAIS_Falling)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

	AttackRange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackRange"));
	AttackRange->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AAttackItem::BeginPlay()
{
	Super::BeginPlay();

	SetItemProperties(ItemStatus);

	//Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AAttackItem::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AAttackItem::OnSphereEndOverlap);
	
	
}

void AAttackItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor)
	{
		AAIProgramCharacter* Program = Cast<AAIProgramCharacter>(OtherActor);
		AAIVaccineCharacter2* Vaccine = Cast<AAIVaccineCharacter2>(OtherActor);
		if (Program || Vaccine)
		{
			bOverlapped = true;

			//spawn overlap particle

			//Spawn overlap begin

			if (Program)
			{
				Program->TakeDamage(100.f);
			}
			if (Vaccine)
			{
				Vaccine->TakeDamage(100.f);
			}

			Destroy();
		}
	}

	
}

void AAttackItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AAIProgramCharacter* Program = Cast<AAIProgramCharacter>(OtherActor);
		AAIVaccineCharacter2* Vaccine = Cast<AAIVaccineCharacter2>(OtherActor);
		if (Program || Vaccine)
		{
			bOverlapped = false;
		}
	}
}

void AAttackItem::StopFalling()
{
	bFalling = false;
	SetAttackItemStatus(EAttackItemStatus::EAIS_Place);
}

void AAttackItem::SetItemProperties(EAttackItemStatus State)
{

	switch (State)
	{
	case EAttackItemStatus::EAIS_Falling:

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

		AttackRange->SetVisibility(false);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttackRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		break;

	case EAttackItemStatus::EAIS_Place:

		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		AttackRange->SetVisibility(true);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EAttackItemStatus::EAIS_Finish:

		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AttackRange->SetVisibility(false);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	}

}

void AAttackItem::ThrowItem()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorScale3D(FVector(1.0f));
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };

	//Direction in which we throw the HealPack
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(160.f, MeshForward);

	float RandomRotation = { 60.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 100.f;

	//Use MeshRight to randomize
	GetItemMesh()->AddImpulse(ImpulseDirection);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, GetItemMesh()->GetName());
	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowItemTimer, this, &AAttackItem::StopFalling, ThrowItemTime);
}

void AAttackItem::SetAttackItemStatus(EAttackItemStatus Status)
{
	ItemStatus = Status;
	SetItemProperties(ItemStatus);
}

// Called every frame
void AAttackItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorScale3D(FVector(0.8f));
}

