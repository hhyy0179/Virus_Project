// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBarWidget.h"
#include "Components/ProgressBar.h"

UHPBarWidget::UHPBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UHPBarWidget::UpdateHPBar(float Value)
{
	HPBar->SetPercent(Value);
}

void UHPBarWidget::SetPercent(float Value)
{
	HPBar->SetPercent(Value);
}
