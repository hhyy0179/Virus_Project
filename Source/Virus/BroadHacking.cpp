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
	BroadHackingStatus(EBroadHackingStatus::EBHS_Finish)
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
	//broadhacking 애니메이션 몽타주 넣기
	SetBroadHackingStatus(EBroadHackingStatus::EBHS_Hacking);
	GetWorldTimerManager().SetTimer(HackingTimer, this, &ABroadHacking::StopHacking, HackingTime);
}


void ABroadHacking::StopHacking()
{
	bAttackOverlapped = false;
	SetBroadHackingStatus(EBroadHackingStatus::EBHS_Finish);
}


void ABroadHacking::SetBroadHackingProperties(EBroadHackingStatus State)
{
	switch (State)
	{
	case EBroadHackingStatus::EBHS_Hacking:

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		
		AttackRange->SetVisibility(true);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EBroadHackingStatus::EBHS_Finish:

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AttackRange->SetVisibility(false);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	}

}

// Called every frame
void ABroadHacking::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Keep the item upright
	if (GetBroadHackingStatus() == EBroadHackingStatus::EBHS_Hacking)
	{
		const FRotator MeshRotation{ 0.f, GetStaticMesh()->GetComponentRotation().Yaw, 0.f };
		GetStaticMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ABroadHacking::SetBroadHackingStatus(EBroadHackingStatus Status)
{
	BroadHackingStatus = Status;
	SetBroadHackingProperties(BroadHackingStatus);
}

