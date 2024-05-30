// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHealthComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTICWARFARE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// play rate of animation
	UPROPERTY(EditDefaultsOnly, Category=Health)
	float PlayRate = 1;
	
	// animation for death
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* DeathAnimation;

	// sound of death
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundBase* DeathSound;

	// plays death animation and sound
	UFUNCTION(NetMulticast, Unreliable)
	void PlayDeathEffects();
	
public:
	// health of player
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category=Health)
	int Health = 50;

	// max health of player
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category=Health)
	int MaxHealth = 100;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Changes health of a character. Pass negative to subtract health.
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AddHealth(int Amount);

	// Delegate to bind to when owner is dead
	UPROPERTY(BlueprintAssignable)
	FDeathDelegate OnDeath;

	// Get Health
	UFUNCTION(BlueprintCallable, Category=Health)
	FORCEINLINE int GetHealth() const { return Health; } ;
		
	// Get Max Health
	UFUNCTION(BlueprintCallable, Category=Health)
	FORCEINLINE int GetMaxHealth() const { return MaxHealth; } ;

	// Get health percentage
	UFUNCTION(BlueprintCallable, Category=Health)
	FORCEINLINE float GetHealthPercentage() const { return static_cast<float>(Health) / MaxHealth; } ;

};
