// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DeathmatchGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICWARFARE_API ADeathmatchGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=ModeConfig)
	float GameDuration = 60.f;
	UPROPERTY(EditAnywhere, Category=ModeConfig)
	int32 ScoreToWin = 10;
	UPROPERTY(EditAnywhere, Category=ModeConfig)
	int32 PlayersToStartGame = 2;

public:
	void PlayerDied(APlayerController* Player, APlayerController* Killer);
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;

	virtual bool ReadyToStartMatch_Implementation() override;
	virtual bool ReadyToEndMatch_Implementation() override;
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
};
