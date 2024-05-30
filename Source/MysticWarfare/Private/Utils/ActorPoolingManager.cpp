#include "Utils/ActorPoolingManager.h"

#include "MysticWarfare/MysticWarfare.h"
#include "Utils/Poolable.h"


void AActorPoolingManager::AddClass(UClass* Class, ENoInstancesHandlingMethod HandlingMethod)
{
	check(Class != nullptr);
	
	ActorBank.Add(Class, TArray<AActor*>());
	HandlingMethods.Add(Class, HandlingMethod);
	if (HandlingMethod == ENoInstancesHandlingMethod::PULL_OLDEST_INSTANCE_BACK)
	{
		ActorsHistory.Add(Class, TArray<AActor*>());
	}
}

AActor* AActorPoolingManager::SpawnActorOfClass(UClass* Class)
{
	check(Class != nullptr);
	
	const FVector PoolLocation = GetActorLocation();
	const FRotator PoolRotation = GetActorRotation();

	UWorld* World = GetWorld();
	check(World != nullptr)
	
	AActor* NewActor = World->SpawnActor(Class, &PoolLocation, &PoolRotation);
	if (NewActor->Implements<UPoolable>())
	{
		IPoolable::Execute_Init(NewActor, this);
	}

	NewActor->SetActorHiddenInGame(true);

	return NewActor;
}

AActorPoolingManager::AActorPoolingManager()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Actor Root"));
	SetRootComponent(Root);
	
	PrimaryActorTick.bCanEverTick = false;
	bNetLoadOnClient = false;
}

void AActorPoolingManager::RegisterActors(const TSubclassOf<AActor>& ClassToRegister, int32 Amount, ENoInstancesHandlingMethod HandlingMethod)
{
	if (Amount <= 0)
	{
		return;
	}
	
	if (!ActorBank.Contains(ClassToRegister))
	{
		AddClass(ClassToRegister, HandlingMethod);
	}
	
	for (int index=0; index < Amount; index++)
	{
		ActorBank[ClassToRegister].Push(SpawnActorOfClass(ClassToRegister));
	}
}

AActor* AActorPoolingManager::GetActorFromPool(const TSubclassOf<AActor>& Class)
{
	if (!ActorBank.Contains(Class))
	{
		UE_LOG(LogUtils, Warning, TEXT("AActorPoolingManager::GetActorFromPool: Trying to get object without a Pool, creating Pool for it"));
		AddClass(Class, ENoInstancesHandlingMethod::SPAWN_NEW_INSTANCE_AND_LOG_WARNING);
	}

	if (ActorBank[Class].IsEmpty())
	{
		switch (HandlingMethods[Class])
		{
		case ENoInstancesHandlingMethod::SPAWN_NEW_INSTANCE:
			ActorBank[Class].Push(SpawnActorOfClass(Class));
			break;
		case ENoInstancesHandlingMethod::SPAWN_NEW_INSTANCE_AND_LOG_WARNING:
			ActorBank[Class].Push(SpawnActorOfClass(Class));
			UE_LOG(LogUtils, Warning, TEXT("AActorPoolingManager::GetActorFromPool: Spawned new actor of class %s"), *Class->GetName());
			break;
		case ENoInstancesHandlingMethod::CRASH:
			UE_LOG(LogUtils, Fatal, TEXT("AActorPoolingManager::GetActorFromPool: Failed for class %s"), *Class->GetName());
			check(false);
			break;
		case ENoInstancesHandlingMethod::PULL_OLDEST_INSTANCE_BACK:
			if (ActorsHistory[Class].IsEmpty())
			{
				UE_LOG(LogUtils, Error, TEXT("AActorPoolingManager::GetActorFromPool: Actor history empty, fallback to spawning actor"));
				ActorBank[Class].Push(SpawnActorOfClass(Class));
			}
			else
			{
				ReturnActorToPool(ActorsHistory[Class][0]);	//Gets deleted from ActorsHistory in ReturnActorToPool
			}
			break;
		}
	}

	AActor* Actor = ActorBank[Class].Pop();
	
	if (Actor->Implements<UPoolable>())
	{
		IPoolable::Execute_OnExitedPool(Actor);
	}

	if (HandlingMethods[Class] == ENoInstancesHandlingMethod::PULL_OLDEST_INSTANCE_BACK)
	{
		ActorsHistory[Class].Push(Actor);
	}
	
	Actor->SetActorHiddenInGame(false);
	return Actor;
}

void AActorPoolingManager::ReturnActorToPool(AActor* Actor)
{
	if (Actor == nullptr)
	{
		return;
	}
	
	UClass* ActorClass = Actor->GetClass();

	if (!ActorBank.Contains(ActorClass))
	{
		UE_LOG(LogUtils, Warning, TEXT("AActorPoolingManager::ReturnActorToPool: Trying to return object without a Pool, creating Pool for it"));
		AddClass(ActorClass, ENoInstancesHandlingMethod::SPAWN_NEW_INSTANCE_AND_LOG_WARNING);
	}

	if (Actor->Implements<UPoolable>())
	{
		IPoolable::Execute_OnReturnedToPool(Actor);
	}

	if (HandlingMethods[ActorClass] == ENoInstancesHandlingMethod::PULL_OLDEST_INSTANCE_BACK)
	{
		ActorsHistory[ActorClass].Remove(Actor);
	}

	ActorBank[ActorClass].Push(Actor);
	Actor->SetActorLocation(GetActorLocation());
	Actor->SetActorRotation(GetActorRotation());
	Actor->SetActorHiddenInGame(true);
}

int32 AActorPoolingManager::GetNumOfRegisteredObjects() const
{
	int32 Result = 0;

	for (const TTuple<UClass*, TArray<AActor*>>& Elem : ActorBank)
	{
		Result += Elem.Value.Num();
	}
		
	return Result;
}

FString AActorPoolingManager::GetContainString() const
{
	FString Result(TEXT("Bank contains:"));

	for (const TTuple<UClass*, TArray<AActor*>>& Elem : ActorBank)
	{
		Result.Append("\n- ");
		Result.Append(Elem.Key->GetName());
		Result.Append(": ");
		Result.AppendInt(Elem.Value.Num());
	}
		
	return Result;
}

