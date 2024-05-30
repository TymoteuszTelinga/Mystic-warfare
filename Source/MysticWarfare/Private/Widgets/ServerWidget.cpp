// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ServerWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MWGameInstance.h"

void UServerWidget::NativeConstruct()
{
	
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UServerWidget::Join);
	}
	if (ServerName)
	{
		ServerName->SetText(FText::FromString(ServerInfo.GetSessionIdStr()));
	}
}

void UServerWidget::Join()
{
	UMWGameInstance* GameInstance = Cast<UMWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance))
	{
		GameInstance->JoinServer(ServerInfo);
	}
}
