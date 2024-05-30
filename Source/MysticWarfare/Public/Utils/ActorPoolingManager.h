#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorPoolingManager.generated.h"

UENUM(BlueprintType)
enum class ENoInstancesHandlingMethod : uint8
{
	SPAWN_NEW_INSTANCE					UMETA(DisplayName="Spawn New Instance"),
	SPAWN_NEW_INSTANCE_AND_LOG_WARNING	UMETA(DisplayName="Spawn New Instance and Log Warning"),
	CRASH								UMETA(DisplayName="Crash"),
	/** Takes instance that was outside pool longest time, and pushes it to ReturnActorToPool */
	PULL_OLDEST_INSTANCE_BACK			UMETA(DisplayName="Pull Oldest Instance Back")			
};

UCLASS()
class MYSTICWARFARE_API AActorPoolingManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USceneComponent> Root;

	TMap<UClass*, TArray<AActor*>> ActorBank;
	TMap<UClass*, ENoInstancesHandlingMethod> HandlingMethods;
	TMap<UClass*, TArray<AActor*>> ActorsHistory;

	void AddClass(UClass* Class, ENoInstancesHandlingMethod HandlingMethod);
	AActor* SpawnActorOfClass(UClass* Class);
	
public:
	AActorPoolingManager();

	/** Create some amount of Actors in advance */
	UFUNCTION(BlueprintCallable)
	void RegisterActors(const TSubclassOf<AActor>& ClassToRegister, int32 Amount, ENoInstancesHandlingMethod HandlingMethod = ENoInstancesHandlingMethod::SPAWN_NEW_INSTANCE_AND_LOG_WARNING);
	UFUNCTION(BlueprintCallable)
	AActor* GetActorFromPool(const TSubclassOf<AActor>& Class);
	UFUNCTION(BlueprintCallable)
	void ReturnActorToPool(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsClassRegistered(UClass* Class) { return ActorBank.Contains(Class); }
	/** For debug */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetNumOfRegisteredObjects() const;
	/** For debug. Returns string that you can log/print */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetContainString() const;
};
