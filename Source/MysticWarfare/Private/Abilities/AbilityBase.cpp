// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AbilityBase.h"

#include "Kismet/GameplayStatics.h"
#include "MysticWarfare/MysticWarfareCharacter.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogAbilityBase);

// Sets default values for this component's properties
UAbilityBase::UAbilityBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);
	// ...
}

// Called when the game starts
void UAbilityBase::BeginPlay()
{
	Super::BeginPlay();

	DurationLeft = Duration;
}

void UAbilityBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityBase, DurationLeft);
	DOREPLIFETIME(UAbilityBase, CooldownLeft);
	DOREPLIFETIME(UAbilityBase, bInUse);

}


void UAbilityBase::AcivateAbilityClient_Implementation()
{
	ActivateAbility();
}

void UAbilityBase::RServer_EndAbility_Implementation()
{
	OnAbilityEnded();
}

void UAbilityBase::EndAbilityClient_Implementation()
{
	OnAbilityEnded();
}

// Called every frame
void UAbilityBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// only server checking
	if(!GetOwner()->HasAuthority())
		return;
	
	// we are not using ability
	if(!bInUse)
	{
		// we have some cooldown left...
		if(CooldownLeft > 0)
		{
			// decrease cooldown time
			CooldownLeft -= DeltaTime;

			// for debug purposes
			// GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green,
			// 	FString::Printf(TEXT("Cooldown left: %f"), CooldownLeft));
		}

		// we dont have cooldown
		else
		{
			// set to zero - so as not to go under 0
			CooldownLeft = 0;
		}	
	}
	// we are using ability it has duration
	else if(Duration != 0)
	{
		// ability in use
		if(bInUse)
		{
			// decrease duration left
			DurationLeft -= DeltaTime;

			// for debug purposes
			// GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green,
			// 	FString::Printf(TEXT("Time left: %f"), DurationLeft));
		}

		// we have no more duration left
		if(DurationLeft <= 0)
		{
			// Prepare for next ability usage
			DurationLeft = Duration;
			
			// finished using ability
			bInUse = false;

			// end on server 
			if(RemoteCall)
				RServer_EndAbility();
			// end on client
			else 
				EndAbilityClient();

			// set cooldown
			CooldownLeft = CooldownTime;

			// debug purposes
			// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green,
			// 	FString::Printf(TEXT("Finished using ability with duration")));
		}
	}
}

void UAbilityBase::OnAbilityEnded_Implementation()
{
}

void UAbilityBase::PlayEffects_Implementation()
{
	Multicast_PlayEffects();
}

void UAbilityBase::Multicast_PlayEffects_Implementation()
{
	// it is known that character is not null here
	if(const USkeletalMeshComponent* Mesh = WarfareCharacter->GetMesh();
		Mesh != nullptr &&
		AbilityAnimation != nullptr)
	{
		// Mesh has animation instance
		if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
		{
			// play ability animation
			AnimInstance->Montage_Play(AbilityAnimation, PlayRate);
		}
	}

	// get world
	const UWorld* World = GetWorld();

	// we have sound
	if(AbilitySound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(World, AbilitySound, WarfareCharacter->GetActorLocation());
	}
}

bool UAbilityBase::TryActivateAbility_Implementation()
{
	if (bInUse)
	{
		UE_LOG(LogAbilityBase, Warning, TEXT("Ability (Client): You are using me now!"));
		return false;		
	}
	
	// still have cooldown
	if(CooldownLeft > 0)
	{
		UE_LOG(LogAbilityBase, Warning, TEXT("Ability (Client): please, wait some time, I need to rest!"));
		return false;
	}

	// server activation
	RServer_Activate();
	
	return true;
}

void UAbilityBase::RServer_Activate_Implementation()
{
	// if ability in use
	if (bInUse)
	{
		UE_LOG(LogAbilityBase, Warning, TEXT("Ability (Server): You are using me now!"));
		return;		
	}
	
	// still have cooldown
	if(CooldownLeft > 0)
	{
		UE_LOG(LogAbilityBase, Warning, TEXT("Ability (Server): please, wait some time, I need to rest!"));
		return;
	}
	
	// activate ability on server
	if(RemoteCall)
		ActivateAbility();

	// activate on client
	else
		AcivateAbilityClient();
	
	// play animations, sounds, etc.
	Multicast_PlayEffects();

	// if no duration
	if(Duration == 0.f)
		// set cooldown
		CooldownLeft = CooldownTime;

	// if has duration...
	if(Duration > 0)
		// set flag
		bInUse = true;

}

