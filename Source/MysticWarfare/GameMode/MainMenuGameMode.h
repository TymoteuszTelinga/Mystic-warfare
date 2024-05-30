// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <MysticWarfare/MysticWarfareGameMode.h>
#include "Blueprint/UserWidget.h"
#include "MainMenuGameMode.generated.h"


/**
 * Rotates the pawn slowly around, displays the main menu, locks the controls, frees mouse cursor. 
 */
UCLASS()
class MYSTICWARFARE_API AMainMenuGameMode : public AMysticWarfareGameMode
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category="UI")
    UUserWidget* MainMenuWidget;

};
