// Fill out your copyright notice in the Description page of Project Settings.


#include "VirusPlayerController.h"
#include "Blueprint/UserWidget.h"



void AVirusPlayerController::BeginPlay()
{

	Super::BeginPlay();

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
	

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		
	}
}

void AVirusPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->AddToViewport();
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AVirusPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->AddToViewport();
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}
