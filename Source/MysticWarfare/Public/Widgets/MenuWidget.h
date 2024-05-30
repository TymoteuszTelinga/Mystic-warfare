// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

class UVerticalBox;
class UMWGameInstance;
class UServerWidget;
/**
 * 
 */
UCLASS()
class MYSTICWARFARE_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UServerWidget> ServerWidget;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> ServerList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMWGameInstance* GameInstance;

	virtual void NativeConstruct() override;

private:

	UFUNCTION()
	void UpdateServerList();
	
};
