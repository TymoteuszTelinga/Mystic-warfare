// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MenuWidget.h"
#include "Components/VerticalBox.h"
#include "MWGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/ServerWidget.h"

void UMenuWidget::NativeConstruct()
{
	GameInstance = Cast<UMWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance))
	{
		GameInstance->OnSessionSearchComplete.AddDynamic(this, &UMenuWidget::UpdateServerList);
	}
}

void UMenuWidget::UpdateServerList()
{
	if (!IsValid(ServerList))
	{
		return;
	}

	ServerList->ClearChildren();

	TArray<FOnlineSessionSearchResult> Result = GameInstance->GetSearchResoult();

	for (int32 i = 0; i < Result.Num(); i++)
	{

		UServerWidget* CurentWidget = CreateWidget<UServerWidget>(GetOwningPlayer(), ServerWidget);
		if (CurentWidget != nullptr)
		{
			CurentWidget->ServerInfo = Result[i];
			CurentWidget->AddToViewport();
			ServerList->AddChild(CurentWidget);
		}

	}
}
