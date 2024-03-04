// Fill out your copyright notice in the Description page of Project Settings.


#include "BroadHackingSkill.h"
#include "Item.h"
#include "VirusCharacter.h"
#include "Components/SphereComponent.h"
#include "AIVaccineCharacter2.h"
#include "AIProgramCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABroadHackingSkill::ABroadHackingSkill():
	ThrowItemTime(0.1f),
	bFalling(false),
	AttackTime(2.f),
	bAttackOverlapped(false),
	AttackStatus(EBroadHackingSkillStatus::EBHSS_Falling)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

	AttackRange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackRange"));
	AttackRange->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ABroadHackingSkill::BeginPlay()
{
	Super::BeginPlay();

	SetBroadHackingProperties(AttackStatus);

	//Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ABroadHackingSkill::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ABroadHackingSkill::OnSphereEndOverlap);
	
}

void ABroadHackingSkill::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AAIProgramCharacter* Program = Cast<AAIProgramCharacter>(OtherActor);
		AAIVaccineCharacter2* Vaccine = Cast<AAIVaccineCharacter2>(OtherActor);
		if (Program || Vaccine)
		{
			bAttackOverlapped = true;
			if (EnemyHitVFX)
			{
				if (Program)
				{
					Program->TakeDamage(40.f);
					//UGameplayStatics::SpawnEmitterAttached(EnemyHitVFX, Program->GetCapsuleComponent(), NAME_None, Program->GetActorLocation(), FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyHitVFX, Program->GetActorTransform());
				}
				if (Vaccine)
				{
					Vaccine->TakeDamage(40.f);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyHitVFX, Vaccine->GetActorTransform());
				}
			}
			
		}
	}
}

void ABroadHackingSkill::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ABroadHackingSkill::StopFalling()
{
	bFalling = false;
	SetBroadHackingStatus(EBroadHackingSkillStatus::EBHSS_BroadAttack);
	SkillAction();
}

void ABroadHackingSkill::SkillAction()
{
	GetWorldTimerManager().SetTimer(AttackTimer, this, &ABroadHackingSkill::StopAttack, AttackTime);
}

void ABroadHackingSkill::StopAttack()
{
	bAttackOverlapped = false;
	SetBroadHackingStatus(EBroadHackingSkillStatus::EBHSS_Finish);
}

void ABroadHackingSkill::SetBroadHackingProperties(EBroadHackingSkillStatus State)
{
	switch (State)
	{
	case EBroadHackingSkillStatus::EBHSS_Falling:

		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(false);
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

	case EBroadHackingSkillStatus::EBHSS_BroadAttack:

		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set AreaSphere properties 
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		AttackRange->SetVisibility(false);
		AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EBroadHackingSkillStatus::EBHSS_Finish:

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

void ABroadHackingSkill::ThrowItem(float CharacterSpeed)
{

	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorScale3D(FVector(1.0f));
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };

	//Direction in which we throw the HealPack
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(160.f, MeshForward);

	//Use MeshRight to randomize
	GetItemMesh()->AddImpulse(ImpulseDirection);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, GetItemMesh()->GetName());
	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowItemTimer, this, &ABroadHackingSkill::StopFalling, ThrowItemTime);
}

void ABroadHackingSkill::SetBroadHackingStatus(EBroadHackingSkillStatus Status)
{
	AttackStatus = Status;
	SetBroadHackingProperties(AttackStatus);
}

// Called every frame
void ABroadHackingSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const  FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

