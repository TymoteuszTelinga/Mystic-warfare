#include "MysticWarfareGameMode.h"
#include "MysticWarfareCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMysticWarfareGameMode::AMysticWarfareGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
