// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DeathmatchHUD.generated.h"

class UUserWidget;
class APlayerController;
/**
 * 
 */
UCLASS()
class MYSTICWARFARE_API ADeathmatchHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> StartWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GameWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> EndWidget;

	UPROPERTY()
	UUserWidget* CurentWidget;

	UPROPERTY()
	APlayerController* PlayerController;

public:

	UFUNCTION(Reliable, Client)
	void DisplayGameWidget();
	void DisplayEndWidget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
