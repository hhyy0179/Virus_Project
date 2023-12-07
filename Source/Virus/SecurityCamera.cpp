// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityCamera.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
ASecurityCamera::ASecurityCamera():
	bHasTarget(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActorBase = CreateDefaultSubobject<UArrowComponent>(TEXT("ActorBase"));
	ActorBase->ArrowLength = 80.f;
	ActorBase->ScreenSize = 0.0025f;
	ActorBase->SetVisibility(false);
	SetRootComponent(ActorBase);

	CameraBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBase"));
	CameraBase->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBase);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(Camera);
	SpotLight->SetIntensity(5.f);
	SpotLight->SetAttenuationRadius(500.f);
	SpotLight->SetOuterConeAngle(20.f);
	SpotLight->SetSourceRadius(2.f);
	SpotLight->SetCastShadows(false);

	Viewcone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Viewcone"));
	Viewcone->SetupAttachment(Camera);
	Viewcone->OnComponentBeginOverlap.AddDynamic(this, &ASecurityCamera::OnOverlapBegin);

	ViewCasule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ViewCasule"));
	ViewCasule->SetupAttachment(Viewcone);
	ViewCasule->InitCapsuleSize(512.0f, 128.0f);
	ViewCasule->OnComponentEndOverlap.AddDynamic(this, &ASecurityCamera::OnOverlapEnd);

	AudioScanner = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioScanner"));
	AudioScanner->SetupAttachment(Viewcone);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(Camera);


}

// Called when the game starts or when spawned
void ASecurityCamera::BeginPlay()
{

	Super::BeginPlay();
	

}

void ASecurityCamera::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!bHasTarget && OtherActor == PlayerPawn)
	{
		bHasTarget = true;
		AudioHookUp(SwitchScanner, ScannerAlarmSoundCue);

	}

}

void ASecurityCamera::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Called every frame
void ASecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASecurityCamera::AudioHookUp(USoundCue* SwitchSoundCue,USoundBase* ScannerSoundCue)
{
	if(SwitchSoundCue)
	{	
		UGameplayStatics::PlaySoundAtLocation(this, SwitchSoundCue, Camera->GetComponentLocation());
		AudioScanner->SetSound(ScannerSoundCue);
		AudioScanner->Activate(false);
	}
	
}

void ASecurityCamera::SetViewcone(FLinearColor LightColor)
{
	SpotLight->SetLightColor(LightColor, true);
}

