#include "WeaponsSystem/InsertActionWeaponComponent.h"

#include "Kismet/GameplayStatics.h"
#include "MysticWarfare/MysticWarfare.h"
#include "Net/UnrealNetwork.h"


void UInsertActionWeaponComponent::OnReloadFishedDelegateHandler()
{
	if (!bBulletInserted)
	{
		TryInsertBullet();
	}
}

void UInsertActionWeaponComponent::TryInsertBullet()
{
	if (bBulletInserted || GetBulletsInMagazine() < 1 || IsDuringReload())
	{
		return;
	}

	if (InsertDurationMode == EInsertDurationMode::ON_RELOAD_ANIMATION_FINISHED && !IsSkeletalMeshProvided())
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("UInsertActionWeaponComponent::TryInsertBullet: Insert iniciated while skeletal mesh was not provided. Fallback to Fixed Duration"));
		InsertDurationMode = EInsertDurationMode::FIXED_DURATION;
	}

	if (InsertDurationMode == EInsertDurationMode::FIXED_DURATION)
	{
		const UWorld* World = GetWorld();

		if (World == nullptr)
		{
			UE_LOG(LogMWWeaponsSystem, Error, TEXT("UInsertActionWeaponComponent::TryInsertBullet: Cannot access world"));
			return;
		}
		
		World->GetTimerManager().SetTimer(FixedInsertTimerHandle, this, &UInsertActionWeaponComponent::TryInsertBullet, FixedInsertDuration, false);
	}

	Multicast_PlayInsertEffects();
}

void UInsertActionWeaponComponent::FinishInsert()
{
	bBulletInserted = true;
}

void UInsertActionWeaponComponent::Multicast_PlayInsertEffects_Implementation()
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("UInsertActionWeaponComponent::Multicast_PlayInsertEffects_Implementation: Cannot access World"));
		return;
	}

	if (InsertSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(World, InsertSound, GetComponentLocation());
	}

	if (IsSkeletalMeshProvided() && InsertAnimation != nullptr)
	{
		if (UAnimInstance* AnimInstance = GetSkeletalMeshComponent()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(InsertAnimation, InsertAnimPlayRate);
		}
	}
}

void UInsertActionWeaponComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageEnded(Montage, bInterrupted);

	if (Montage == InsertAnimation && !bInterrupted)
	{
		FinishInsert();
	}
}

void UInsertActionWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	OnReloadFinishedDelegate.AddUObject(this, &UInsertActionWeaponComponent::OnReloadFishedDelegateHandler);
}

void UInsertActionWeaponComponent::RServer_Fire_Implementation(const FVector& SpawnLocation,
                                                               const FRotator& SpawnRotation)
{
	Super::RServer_Fire_Implementation(SpawnLocation, SpawnRotation);

	bBulletInserted = false;
	TryInsertBullet();
}

void UInsertActionWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInsertActionWeaponComponent, bBulletInserted, COND_OwnerOnly);
}

UInsertActionWeaponComponent::UInsertActionWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInsertActionWeaponComponent::StartShooting()
{
	if (!TryGetReadyForShot())
	{
		return;
	}
	
	// Call fire if bullet is inserted or if there are no bullets in magazine (to play empty mag sound)
	if (bBulletInserted || GetBulletsInMagazine() < 1)
	{
		Super::StartShooting();
	}
}
