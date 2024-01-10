// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class VIRUS_API UHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UHPBarWidget(const FObjectInitializer& ObjectInitializer);
	void NativeConstruct();

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UProgressBar* HPBar;

	UFUNCTION(BlueprintCallable)
	void UpdateHPBar(float Value);

	UFUNCTION(BlueprintCallable)
	void SetPercent(float Value);
};
