// Fill out your copyright notice in the Description page of Project Settings.


#include "BroadHacking.h"
#include "Components/SphereComponent.h"
#include "AIVaccineCharacter2.h"
#include "AIProgramCharacter.h"

// Sets default values
ABroadHacking::ABroadHacking():
	HackingTime(3.0f),
	CoolTime(15.0f),
	bAttackOverlapped(false),
	BroadHackingStatus(EBroadHackingStatus::EBHS_Falling),
	ThrowItemTime(1.2f),
	bFalling(false),
	bCoolTime(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackRange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackRange"));
	SetRootComponent(AttackRange);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ABroadHacking::BeginPlay()
{
	Super::BeginPlay();
	
	SetBroadHackingProperties(BroadHackingStatus);

	//Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ABroadHacking::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ABroadHacking::OnSphereEndOverlap);
}

void ABroadHacking::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AAIProgramCharacter* Program = Cast<AAIProgramCharacter>(OtherActor);
		AAIVaccineCharacter2* Vaccine = Cast<AAIVaccineCharacter2>(OtherActor);
		if (Program || Vaccine)
		{
			bAttackOverlapped = true;
			if (Program)
			{
				Program->TakeDamage(40.f);
			}
			if (Vaccine)
			{
				Vaccine->TakeDamage(40.f);
			}
		}
	}
}

void ABroadHacking::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AAIProgramCharacter* Program = Cast<AAIProgramCharacter>(OtherActor);
		AAIVaccineCharacter2* Vaccine = Cast<AAIVaccineCharacter2>(OtherActor);
		if (Program || Vaccine)
		{
			bAttackOverlapped = false;
		}
	}
}


void ABroadHacking::HackingAction()
{
	SetBroadHackingStatus(EBroadHackingStatus::EBHS_BroadHacking);
	GetWorldTimerManager().SetTimer(HackingTimer, this, &ABroadHacking::StopHacking, HackingTime);
}


void ABroadHacking::StopHacking()
{
	bAttackOverlapped = false;
	SetBroadHackingStatus(EBroadHackingStatus::EBHS_Finish);
}

void ABroadHacking::StopFalling()
{
	bFalling = false;
	HackingAction();
}


void ABroadHacking::SetBroadHackingProperties(EBroadHackingStatus State)
{
	switch (State)
	{
	case EBroadHackingStatus::EBHS_Falling:

		AttackRange->SetSimulatePhysics(true);
		AttackRange->SetEnableGravity(true);
		AttackRange->SetVisibility(true);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttackRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	case EBroadHackingStatus::EBHS_BroadHacking:

		AttackRange->SetSimulatePhysics(false);
		AttackRange->SetEnableGravity(false);
		AttackRange->SetVisibility(true);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;

	case EBroadHackingStatus::EBHS_Finish:

		//Set Mesh Properties
		AttackRange->SetSimulatePhysics(false);
		AttackRange->SetEnableGravity(false);
		AttackRange->SetVisibility(false);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	}

}

// Called every frame
void ABroadHacking::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Keep the item upright
	if (GetBroadHackingStatus() == EBroadHackingStatus::EBHS_BroadHacking)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ABroadHacking::SetBroadHackingStatus(EBroadHackingStatus Status)
{
	BroadHackingStatus = Status;
	SetBroadHackingProperties(BroadHackingStatus);
}

void ABroadHacking::ThrowAttackRange()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };

	//Direction in which we throw the HealPack
	/* Legacy */
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(90.f, MeshForward);

	/* New */
	//FVector ImpulseDirection = MeshForward.RotateAngleAxis(20.f, MeshRight);

	float RandomRotation = { 30.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 1'000.f;

	//Use MeshRight to randomize
	GetItemMesh()->AddImpulse(ImpulseDirection);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, GetItemMesh()->GetName());
	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowItemTimer, this, &ABroadHacking::StopFalling, ThrowItemTime);
}


