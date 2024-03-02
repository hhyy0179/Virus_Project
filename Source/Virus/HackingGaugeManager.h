// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HackingGaugeManager.generated.h"

UCLASS()
class VIRUS_API AHackingGaugeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHackingGaugeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ControlGauge(float Value);

	int Count = 0; // 60fps의 경우 600까지 게이지가 그대로면 ally->> program 돌아가게
	int MaxCount = 600;
	TSubclassOf<UUserWidget> HackingGauageClass;
	UUserWidget* HackingGauageWidget;

	TSubclassOf<UUserWidget> GameClearClass;
	UUserWidget* GameClearWidget;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Percent = 0.0f; // 퍼센트를 정수 값으로 표시

	float PreviousPercent = 0.0f;
	float FixedPercent[6] = { 0.0f, 0.1f, 0.25f, 0.5f, 0.7f, 0.8f };
	int CurrentPercentIndex = 0;
};
