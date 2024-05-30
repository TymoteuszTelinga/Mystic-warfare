// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Knockback.generated.h"

// this class does not need to be modified
UINTERFACE(MinimalAPI, BlueprintType)
class UKnockback : public UInterface
{
	GENERATED_BODY()
};

// interface for every actor that wants to interact with knockback ability
class MYSTICWARFARE_API IKnockback
{
	GENERATED_BODY()

  public:

	// Characters implementing that event are reacting to Knockback Ability
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability Reactions")
	void ReactToKnockback(FVector LaunchVector);

};