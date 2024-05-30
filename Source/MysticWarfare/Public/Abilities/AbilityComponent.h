// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AbilityComponent.generated.h"

class AMysticWarfareCharacter;
class UAbilityBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTICWARFARE_API UAbilityComponent : public USceneComponent
{
	GENERATED_BODY()

protected:

	// Array of abilities
	UPROPERTY(EditAnywhere, Category=Abilities)
	TArray<UAbilityBase*> Abilities;

public:	
	// Sets default values for this component's properties
	UAbilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Activate ability of specified index
	UFUNCTION(BlueprintCallable, Category=Abilities)
	void ActivateAbility(const int Index);

	// Adds ability to array
	UFUNCTION(BlueprintCallable, Category=Abilities)
	void AddAbility(UAbilityBase* Ability);
};
