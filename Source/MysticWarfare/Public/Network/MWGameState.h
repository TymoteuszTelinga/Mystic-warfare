#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MWGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputModeChangedSignature);

UENUM(Blueprintable, BlueprintType)
enum class EInputModes : uint8
{
	UIOnly,
	GameOnly,
	GameAndUI
};

UCLASS()
class MYSTICWARFARE_API AMWGameState : public AGameState
{
	GENERATED_BODY()
	
	UPROPERTY()
	APlayerState* BestPlayer = nullptr;
	UPROPERTY()
	EInputModes CurrentInputMode = EInputModes::GameOnly;
	UPROPERTY()
	bool bShowCursor = false;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnInputModeChangedSignature OnInputModeChangedDelegate;
	
	void UpdateBestPlayer(APlayerState* Player);
	void SetInputMode(EInputModes Mode, bool ShowCursor);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EInputModes GetCurrentInputMode() const { return CurrentInputMode; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetShowCursor() const { return bShowCursor; }
};
