// Fill out your copyright notice in the Description page of Project Settings.


#include <MysticWarfare/GameMode/MainMenuGameMode.h>
#include "Kismet/GameplayStatics.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "MysticWarfare/MysticWarfare.h"


void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (MainMenuWidgetClass == nullptr)
    {
        UE_LOG(LogMWNetworking, Error, TEXT("Main Menu Widget Class was not set in Blueprint deriving from AMainMenuGameMode."));
        return;
    }

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0))
        {
            PlayerController->SetInputMode(FInputModeUIOnly());
            PlayerController->bShowMouseCursor = true;

            MainMenuWidget = CreateWidget<UUserWidget>(World, MainMenuWidgetClass);
            MainMenuWidget->AddToViewport();
        }
        else
        {
            UE_LOG(LogMWNetworking, Error, TEXT("AMainMenuGameMode instance could not access the Player Controller object."));
            return;
        }
    }
    else
    {
        UE_LOG(LogMWNetworking, Error, TEXT("AMainMenuGameMode instance could not access the World object."));
        return;
    }
}

