// Fill out your copyright notice in the Description page of Project Settings.


#include "MWGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MysticWarfare/MysticWarfare.h"

UMWGameInstance::UMWGameInstance()
{
}

TArray<FOnlineSessionSearchResult> UMWGameInstance::GetSearchResoult()
{
	return SessionSearch->SearchResults;
}

void UMWGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMWGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMWGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMWGameInstance::OnJoinSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMWGameInstance::OnDestroySessionComplete);
		}
	}
}

void UMWGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	if (Succeeded)
	{
		UWorld* World = GetWorld();
		if (World == nullptr)
		{
			UE_LOG(LogMWNetworking, Error, TEXT("UMWGameInstance::OnCreateSessionComplete: Could not access world"));
			return;
		}

		const FString LevelName = MAP_FOLDER + OpeningSessionLevelName + TEXT("?listen");
		
		World->ServerTravel(LevelName,true);
		UE_LOG(LogMWNetworking, Warning, TEXT("ServerStart"));
	}
}

void UMWGameInstance::OnFindSessionComplete(bool Succeeded)
{
	OnSessionSearchComplete.Broadcast();
}

void UMWGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PController))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "")
		{
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UMWGameInstance::OnDestroySessionComplete(FName SessionName, bool Succeeded)
{
	if (Succeeded)
	{
		const UWorld* World = GetWorld();
		if (World == nullptr)
		{
			UE_LOG(LogMWNetworking, Error, TEXT("UMWGameInstance::OnDestroySessionComplete: Could not access world"));
			return;
		}

		const FString LevelName = MAP_FOLDER + MainMenuLevelName;
		
		UGameplayStatics::OpenLevel(World, FName(LevelName));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("SesionEnded")));
	}
}

void UMWGameInstance::CreateServer(const FString& SessionLevelName)
{
	OpeningSessionLevelName = SessionLevelName;
	
	UE_LOG(LogMWNetworking, Warning, TEXT("Create Server"));
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 2;

	SessionInterface->CreateSession(0, FName("Create Session"), SessionSettings);
}

void UMWGameInstance::FindServer()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMWGameInstance::DestroyServer()
{
	SessionInterface->DestroySession(FName("Create Session"));
}

void UMWGameInstance::JoinServer(FOnlineSessionSearchResult& ServerToJoin)
{
	SessionInterface->JoinSession(0, FName("Create Session"), ServerToJoin);
}

