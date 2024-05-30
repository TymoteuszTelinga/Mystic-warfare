#include "MysticWarfarePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "MWGameInstance.h"

void AMysticWarfarePlayerController::ClientReturnToMainMenuWithTextReason_Implementation(const FText& ReturnReason)
{
	Super::ClientReturnToMainMenuWithTextReason_Implementation(ReturnReason);

	UMWGameInstance* GI = Cast<UMWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GI))
	{
		GI->DestroyServer();
	}

}

void AMysticWarfarePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
