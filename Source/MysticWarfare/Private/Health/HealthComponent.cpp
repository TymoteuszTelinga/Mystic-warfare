// Fill out your copyright notice in the Description page of Project Settings.


#include "Health/HealthComponent.h"

#include "Kismet/GameplayStatics.h"
#include "MysticWarfare/MysticWarfareCharacter.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogHealthComponent);

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, MaxHealth);
}

void UHealthComponent::PlayDeathEffects_Implementation()
{
	const AMysticWarfareCharacter* Character = Cast<AMysticWarfareCharacter>(GetOwner());
	// it is known that character is not null here
	if(const USkeletalMeshComponent* Mesh = Character->GetMesh();
		Mesh != nullptr &&
		DeathAnimation != nullptr)
	{
		// Mesh has animation instance
		if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
		{
			// play ability animation
			AnimInstance->Montage_Play(DeathAnimation, PlayRate);
		}
	}

	// get world
	const UWorld* World = GetWorld();

	// we have sound
	if(DeathSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(World, DeathSound, Character->GetActorLocation());
	}
}

void UHealthComponent::AddHealth_Implementation(int Amount)
{
	// check if health is not to big
	if(Health + Amount > MaxHealth)
	{
		Health = MaxHealth;
		return;
	}

	// check if we are dead
	if(Health + Amount < 0)
	{
		Health = 0;
		PlayDeathEffects();
		OnDeath.Broadcast();
		UE_LOG(LogHealthComponent, Warning, TEXT("Dead"));
		return;
	}

	Health += Amount;
	UE_LOG(LogHealthComponent, Warning, TEXT("Health: %d"), Health);
}
