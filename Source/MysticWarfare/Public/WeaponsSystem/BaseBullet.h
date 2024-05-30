// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBullet.generated.h"

UCLASS(Abstract)
class MYSTICWARFARE_API ABaseBullet : public AActor
{
	GENERATED_BODY()

public:
	ABaseBullet() = default;

	UFUNCTION(BlueprintNativeEvent)
	void Fire(const FRotator& Direction, const AActor* Shooter);
};
