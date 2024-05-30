// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LifeSteal.generated.h"

// this class does not need to be modified
UINTERFACE(MinimalAPI, BlueprintType)
class ULifeSteal : public UInterface
{
	GENERATED_BODY()
};

// interface for every actor that wants to interact with knockback ability
class MYSTICWARFARE_API ILifeSteal
{
	GENERATED_BODY()

public:

	// Characters implementing that event are reacting to Knockback Ability
	// Life taken in health points
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability Reactions")
	void ReactToLifeSteal(float LifeTaken);

};