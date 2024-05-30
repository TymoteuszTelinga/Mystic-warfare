// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticWarfare/GameMode/DeathmatchGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "MysticWarfare/MysticWarfare.h"
#include "Widgets/DeathmatchHUD.h"
#include "Network/MWGameState.h"

void ADeathmatchGameMode::PlayerDied(APlayerController* Player, APlayerController* Killer)
{
	//Update Killer Score
	if (Player != Killer && IsValid(Player) && IsValid(Killer))
	{
		APlayerState* KillerState = Killer->PlayerState;
		KillerState->Score += 1;
		AMWGameState* GS = Cast<AMWGameState>(GameState);
		GS->UpdateBestPlayer(KillerState);
		if (KillerState->GetScore() >= ScoreToWin)
		{
			EndMatch();
		}
	}

	//Respawn Player
	RestartPlayer(Player);
}

void ADeathmatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	UE_LOG(LogMWNetworking, Warning, TEXT("NewPalyer, %d"), NumPlayers);
}

void ADeathmatchGameMode::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("MaxPlayers %f"), GameSession->MaxPlayers));
}

bool ADeathmatchGameMode::ReadyToStartMatch_Implementation()
{
	Super::ReadyToStartMatch_Implementation();
	
	return NumPlayers == PlayersToStartGame;
}

bool ADeathmatchGameMode::ReadyToEndMatch_Implementation()
{
	Super::ReadyToEndMatch_Implementation();

	AGameState* DMGameState = Cast<AGameState>(GameState);
	if (DMGameState)
	{
		return DMGameState->ElapsedTime >= GameDuration;
	}
	return false;
}

void ADeathmatchGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (AMWGameState* MWGameState = Cast<AMWGameState>(GameState))
	{
		MWGameState->SetInputMode(EInputModes::UIOnly, true);
	}
}

void ADeathmatchGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (AMWGameState* MWGameState = Cast<AMWGameState>(GameState))
	{
		MWGameState->SetInputMode(EInputModes::GameOnly, false);
	}
	
	for (const APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (APlayerController* PC = PlayerState->GetPlayerController())
		{
			if (ADeathmatchHUD* HUD = PC->GetHUD<ADeathmatchHUD>())
			{
				HUD->DisplayGameWidget();
			}
		}
	}
}

void ADeathmatchGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasStarted();

	GameSession->ReturnToMainMenuHost();
}
