// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VirusGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class VIRUS_API UVirusGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UVirusGameInstance();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MouseSensitivity = 0.9f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SoundVolume = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString UserName;
};
