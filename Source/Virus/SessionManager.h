// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionManager.generated.h"

UCLASS()
class VIRUS_API ASessionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASessionManager();
	IOnlineSessionPtr OnlineSessionInterface;

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();

	void RefreshServerList();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void CreateGameSession();

	UFUNCTION(BlueprintCallable)
	void JoinGameSession(const FString& Address);

	void OnCreateSessionComplete(FName _SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName _SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName _SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName _SessionName, bool bWasSuccessful);

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UFUNCTION(BlueprintCallable)
	void CheckSessionSearch();

private:
	TSubclassOf<class UUserWidget> TestWidgetClass;

	class UTestWidget* TestWidget;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate	 FindSessionCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;

	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FDelegateHandle StartSessionCompleteDelegateHandle;
};
