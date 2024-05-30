// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/AbilityComponent.h"
#include "Abilities/AbilityBase.h"

// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityComponent::ActivateAbility(const int Index)
{
	if(Index >= Abilities.Num() || Index < 0)
	{
		UE_LOG(LogAbilityBase, Warning, TEXT("index out of bounds"));
		return;
	}

	if(Abilities[Index] == nullptr)
	{
		UE_LOG(LogAbilityBase, Warning, TEXT("this ability was not set (nullptr)"));
		return;
	}
	
	UAbilityBase* Base = Cast<UAbilityBase>(Abilities[Index]);
	if(!Base->TryActivateAbility())
		UE_LOG(LogAbilityBase, Warning, TEXT("Activating ability failed"));
		
}

void UAbilityComponent::AddAbility(UAbilityBase* Ability)
{
	Abilities.Add(Ability);
}



