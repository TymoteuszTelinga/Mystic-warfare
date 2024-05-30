// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "ServerWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class MYSTICWARFARE_API UServerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnlineSessionSearchResult ServerInfo;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ServerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

	//UMultiPlayerGameInstance* GameInstance;

	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void Join();
};
