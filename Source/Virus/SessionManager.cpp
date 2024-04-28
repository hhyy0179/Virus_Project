// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionManager.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "TestWidget.h"

const static FName SESSION_NAME = TEXT("My Session Game");

ASessionManager::ASessionManager()
{
	ConstructorHelpers::FClassFinder<UUserWidget> TestWidgetBPClass(TEXT("/Game/_VirusGame/HUD/WBP_TestWidget"));
	if (!ensure(TestWidgetBPClass.Class != nullptr)) return;

	TestWidgetClass = TestWidgetBPClass.Class;

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void ASessionManager::LoadMenuWidget()
{
	if (!ensure(TestWidgetClass != nullptr)) return;

	TestWidget = CreateWidget<UTestWidget>(GetWorld(), TestWidgetClass);
	
	if (!ensure(TestWidget != nullptr)) return;

	TestWidget->AddToViewport();
}

void ASessionManager::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid()) {
		// SessionSearch->bIsLanQuery = true;
		OnlineSessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

		UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
	}
}

void ASessionManager::BeginPlay()
{
	Super::BeginPlay();

	OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ASessionManager::OnCreateSessionComplete);
	OnlineSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ASessionManager::OnFindSessionComplete);
	OnlineSessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ASessionManager::OnJoinSessionComplete);
	OnlineSessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ASessionManager::OnDestroySessionComplete);
	OnlineSessionInterface->OnStartSessionCompleteDelegates.AddUObject(this, &ASessionManager::OnStartSessionComplete);

	LoadMenuWidget();
}

void ASessionManager::CreateGameSession()
{
	if (!OnlineSessionInterface.IsValid()) return;

	auto ExistingSession = OnlineSessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(SESSION_NAME);
	}
	else
	{
		OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;

		OnlineSessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void ASessionManager::JoinGameSession(const FString& Address)
{
	// TestWidget->SetServerList({ "Test1", "Test2" });
}

void ASessionManager::OnCreateSessionComplete(FName _SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(FString("/Game/BasicMap?listen"));
		}
	}
	else UE_LOG(LogTemp, Warning, TEXT("Failed to Create Session"));
}

void ASessionManager::OnFindSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch.IsValid() && TestWidget != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));
		TArray<FString> ServerNames;

		for (const FOnlineSessionSearchResult& t : SessionSearch->SearchResults) {
			UE_LOG(LogTemp, Warning, TEXT("Found Session Names: %s"), *t.GetSessionIdStr());
			ServerNames.Add(*t.GetSessionIdStr());
		}

		TestWidget->SetServerList(ServerNames);
	}
}

void ASessionManager::OnJoinSessionComplete(FName _SessionName, EOnJoinSessionCompleteResult::Type Result)
{

}

void ASessionManager::OnDestroySessionComplete(FName _SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful) CreateGameSession();
}

void ASessionManager::OnStartSessionComplete(FName _SessionName, bool bWasSuccessful)
{
}

void ASessionManager::CheckSessionSearch()
{
}


