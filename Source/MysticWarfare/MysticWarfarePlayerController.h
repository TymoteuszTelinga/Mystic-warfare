#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MysticWarfarePlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class MYSTICWARFARE_API AMysticWarfarePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

public:
	virtual void ClientReturnToMainMenuWithTextReason_Implementation(const FText& ReturnReason) override;

protected:

	virtual void BeginPlay() override;
};
