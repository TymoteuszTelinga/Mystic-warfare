// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MWGameInstance.generated.h"


// Delegate signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionSearchCompleteSignature);
/**
 * 
 */
UCLASS()
class MYSTICWARFARE_API UMWGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	const FString MAP_FOLDER = TEXT("/Game/MysticWarfare/Maps/");
	
	UPROPERTY(EditAnywhere, Category=Traveling)
	FString MainMenuLevelName = TEXT("MainMenuMap");

	UPROPERTY()
	FString OpeningSessionLevelName;
	
public:
	UMWGameInstance();

	TArray<FOnlineSessionSearchResult> GetSearchResoult();

	UFUNCTION(BlueprintCallable)
	void DestroyServer();

	void JoinServer(FOnlineSessionSearchResult& ServerToJoin);
	
protected:
	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnDestroySessionComplete(FName SessionName, bool Succeeded);

	UFUNCTION(BlueprintCallable)
	void CreateServer(const FString& SessionLevelName);
	UFUNCTION(BlueprintCallable)
	void FindServer();

public:
	UPROPERTY(BlueprintAssignable)
	FOnSessionSearchCompleteSignature OnSessionSearchComplete;

protected:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	IOnlineSessionPtr SessionInterface;
};
