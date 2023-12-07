// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SecurityCamera.generated.h"

UCLASS()
class VIRUS_API ASecurityCamera : public AActor
{
	GENERATED_BODY()

	// Sets default values for this actor's properties
	ASecurityCamera();

	/** Root Component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ActorBase;

	/** CameraBase  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CameraBase;

	/** Camera  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SpotLight, meta = (AllowPrivateAccess = "true"))
	class USpotLightComponent* SpotLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Viewcone, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Viewcone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Viewcone, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* ViewCasule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Viewcone, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioScanner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Viewcone, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scanning, meta = (AllowPrivateAccess = "true"))
	float CameraMaxYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scanning, meta = (AllowPrivateAccess = "true"))
	float CameraPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpotLight, meta = (AllowPrivateAccess = "true"))
	float LightAttenuationRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpotLight, meta = (AllowPrivateAccess = "true"))
	float SpotLightBrightness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpotLight, meta = (AllowPrivateAccess = "true"))
	float LightColorAlarmOff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpotLight, meta = (AllowPrivateAccess = "true"))
	float LightColorAlarmOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targeting, meta = (AllowPrivateAccess = "true"))
	bool bHasTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	bool bInRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
	bool bMusicChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* SwitchScanner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundBase* ScannerAlarmSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundBase* ScannerResetSoundCue;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AudioHookUp(USoundCue* SwitchSoundCue, USoundBase* ScannerSoundCue);

	UFUNCTION(BlueprintCallable)
	void SetViewcone(FLinearColor LightColor);


};
