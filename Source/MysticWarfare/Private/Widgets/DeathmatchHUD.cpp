// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/DeathmatchHUD.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameState.h"

void ADeathmatchHUD::DisplayGameWidget_Implementation()
{
	if (IsValid(CurentWidget))
	{
		CurentWidget->RemoveFromViewport();
	}

	CurentWidget = CreateWidget<UUserWidget>(PlayerController, GameWidget);
	if (IsValid(CurentWidget))
	{
		CurentWidget->AddToViewport();
	}
}

void ADeathmatchHUD::DisplayEndWidget()
{
	if (IsValid(CurentWidget))
	{
		CurentWidget->RemoveFromViewport();
	}

	CurentWidget = CreateWidget<UUserWidget>(PlayerController, EndWidget);
	if (IsValid(CurentWidget))
	{
		CurentWidget->AddToViewport();
	}
}

void ADeathmatchHUD::BeginPlay()
{
	PlayerController = Cast<APlayerController>(GetOwner());

	
	AGameState* GameState = Cast<AGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		if (GameState->IsMatchInProgress())
		{
			DisplayGameWidget();
			return;
		}
	}

	CurentWidget = CreateWidget<UUserWidget>(PlayerController, StartWidget);
	if (IsValid(CurentWidget))
	{
		CurentWidget->AddToViewport();
	}
}
