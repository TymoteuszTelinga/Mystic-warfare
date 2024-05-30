// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/MWGameState.h"
#include "Network/MWPlayerState.h"

void AMWGameState::UpdateBestPlayer(APlayerState* Player)
{
	if (IsValid(BestPlayer))
	{
		if (Player->GetScore() > BestPlayer->GetScore())
		{
			BestPlayer = Player;
		}
	}
	else
	{
		BestPlayer = Player;
	}
}

void AMWGameState::SetInputMode(EInputModes Mode, bool ShowCursor)
{
	CurrentInputMode = Mode;
	bShowCursor = ShowCursor;
	OnInputModeChangedDelegate.Broadcast();
}
