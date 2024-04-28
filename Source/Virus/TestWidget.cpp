// Fill out your copyright notice in the Description page of Project Settings.


#include "TestWidget.h"
#include "ServerRow.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

UTestWidget::UTestWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/_VirusGame/HUD/WBP_ServerRow"));
	ServerRowClass = ServerRowBPClass.Class;
}

void UTestWidget::SetServerList(TArray<FString> ServerNames)
{
	ServerList->ClearChildren();

	for (const FString& ServerName : ServerNames)
	{

		UServerRow* Row = CreateWidget<UServerRow>(GetWorld(), ServerRowClass);

		Row->ServerName->SetText(FText::FromString(ServerName));
		ServerList->AddChild(Row);
	}
}

void UTestWidget::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Server"));
}
