// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbilityBase, Log, All);

class AMysticWarfareCharacter;

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTICWARFARE_API UAbilityBase : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	
	// How much time of ability left
	UPROPERTY(Replicated)
	float DurationLeft = 0;
	
	// how much cooldown left in seconds
	UPROPERTY(Replicated)
	float CooldownLeft = 0;
	
public:	
	// Sets default values for this component's properties
	UAbilityBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Replicate some variables
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Character which ability belongs to
	UPROPERTY(BlueprintReadOnly, Category=Ability)
	TObjectPtr<AMysticWarfareCharacter> WarfareCharacter = nullptr;

	// Animation when ability is in use
	UPROPERTY(EditAnywhere, Category=Animation)
	TObjectPtr<UAnimMontage> AbilityAnimation = nullptr;

	// Sound played when ability is in use
	UPROPERTY(EditAnywhere, Category=Sounds)
	TObjectPtr<USoundBase> AbilitySound = nullptr;

	// cooldown of ability in seconds
	UPROPERTY(EditDefaultsOnly, Category=Ability)
	float CooldownTime = 30;

	// A flag indicating whether ability is in use
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bInUse = false;

	// How long ability lasts. Set zero if has no duration
	UPROPERTY(EditDefaultsOnly, Category=Ability)
	float Duration = 0;

	// set false if ability should be called locally
	UPROPERTY(EditDefaultsOnly, Category=Ability)
	bool RemoteCall = true;

	UFUNCTION(Client, Reliable)
	void AcivateAbilityClient();
	
	// Uses ability 
	UFUNCTION(BlueprintImplementableEvent, Category=Ability)
	void ActivateAbility();

	// called when ability with duration ends
	UFUNCTION(BlueprintNativeEvent, Category=Ability)
	void OnAbilityEnded();

	// Called when ability used locally, but animation will be shown via multicast
	UFUNCTION(Server, Unreliable)
	void PlayEffects();

	// Called when ability should end on server
	UFUNCTION(Server, Reliable)
	void RServer_EndAbility();

	// Called when ability should end on client
	UFUNCTION(Client, Reliable)
	void EndAbilityClient();
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// play rate of animation
	UPROPERTY(EditAnywhere, Category=Ability)
	float PlayRate = 1.f;
		
	// Tries activating ability
	// If can - activates and returns true, if not returns false
	UFUNCTION(BlueprintNativeEvent, Category=Ability)
	bool TryActivateAbility();
	
	// Activate activity. 
	UFUNCTION(Server, Reliable)
	virtual void RServer_Activate();

	// Play animation and sound of ability
	UFUNCTION(NetMulticast, Unreliable)
	virtual void Multicast_PlayEffects();

	// assigns character
	UFUNCTION(BlueprintCallable, Category=Ability)
	void AssignCharacter(AMysticWarfareCharacter* Character) {WarfareCharacter = Character; };
	
	// get remaining time of cooldown
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCooldownLeft() const {return CooldownLeft;};

	// get cooldown time for ability
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCooldownTime() const {return CooldownTime;};

	// get cooldown time left in percentage
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCooldownTimeLeftInPercentage() const {return CooldownLeft / CooldownTime;};
	
	// get remaining time of ability
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAbilityDurationLeft() const {return DurationLeft;};

	// get time of ability
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAbilityDuration() const {return Duration;};

	// get cooldown time left in percentage
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAbilityDurationLeftInPercentage() const {return DurationLeft / Duration;};
};
