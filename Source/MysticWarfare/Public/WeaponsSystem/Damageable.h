#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that interacts with bullet. Objects implementing that interface can be damaged
 */
class MYSTICWARFARE_API IDamageable
{
	GENERATED_BODY()

public:
	/** Here bullet passes amount of damage it caused. Implement according to specific actor */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplyDamage(float Amount);
};
