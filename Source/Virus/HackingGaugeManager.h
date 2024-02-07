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
	void ControlGauge(float Value);
	int Count = 0; // 60fps�� ��� 600���� �������� �״�θ� ally->> program ���ư���
	int MaxCount = 1500;
	TSubclassOf<UUserWidget> HackingGauageClass;
	UUserWidget* HackingGauageWidget;
public:
	float Percent = 0.0f; // �ۼ�Ʈ�� ���� ������ ǥ��
	float PreventPercent = 0.0f;
};
