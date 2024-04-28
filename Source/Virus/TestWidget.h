// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestWidget.generated.h"

/**
 * 
 */
UCLASS()
class VIRUS_API UTestWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTestWidget(const FObjectInitializer& ObjectInitializer);
	TSubclassOf<class UUserWidget> ServerRowClass;

	void SetServerList(TArray<FString> ServerNames);

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

private:

	UFUNCTION(BlueprintCallable)
	void JoinServer();

};
