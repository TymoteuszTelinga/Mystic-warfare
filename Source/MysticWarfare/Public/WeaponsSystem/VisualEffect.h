#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VisualEffect.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UVisualEffect : public UInterface
{
	GENERATED_BODY()
};


class MYSTICWARFARE_API IVisualEffect
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Play(const FVector& Direction);
};
