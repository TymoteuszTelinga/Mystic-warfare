// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Freeze.generated.h"

// this class does not need to be modified
UINTERFACE(MinimalAPI, BlueprintType)
class UFreeze : public UInterface
{
	GENERATED_BODY()
};

// interface for every actor that wants to interact with knockback ability
class MYSTICWARFARE_API IFreeze
{
	GENERATED_BODY()

public:

	// Characters implementing that event are reacting to Knockback Ability
	// time is in seconds
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability Reactions")
	void ReactToFreeze(float time);

};