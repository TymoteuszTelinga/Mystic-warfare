#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Poolable.generated.h"

class AActorPoolingManager;

// This class does not need to be modified.
UINTERFACE()
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Actors that implement that interface have additional callbacks when working with AActorPoolingManager
 */
class MYSTICWARFARE_API IPoolable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Called by AActorPoolingManager in RegisterActors function, when pre-creating Actor */
	UFUNCTION(BlueprintNativeEvent)
	void Init(AActorPoolingManager* OwnAPM);
	/** Called whenever Actor returns to Pool */
	UFUNCTION(BlueprintNativeEvent)
	void OnReturnedToPool();
	/** Called whenever Actor exits Pool */
	UFUNCTION(BlueprintNativeEvent)
	void OnExitedPool();
};
