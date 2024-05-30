#include "WeaponsSystem/AutomaticWeaponComponent.h"

#include "MysticWarfare/MysticWarfare.h"
#include "Net/UnrealNetwork.h"


void UAutomaticWeaponComponent::OnRep_bShotReady()
{
	if (bShotReady && bTriggerDown)
	{
		Super::StartShooting();
	}
}

void UAutomaticWeaponComponent::CooldownEnded()
{
	bShotReady = true;
	OnRep_bShotReady();
}

void UAutomaticWeaponComponent::RServer_Fire_Implementation(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	const UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("UAutomaticWeaponComponent::RServer_Fire_Implementation: Cannot access World"));
		return;
	}

	// Check is before fire because we want one dry fire sound at the end of full auto, but only once
	// Without check it would repeat dry fire sound until character releases trigger
	// If check was after fire, there wouldn't be dry fire sound at all
	const bool WasBulletInMagazineBeforeShot = GetBulletsInMagazine() > 0;
	
	Super::RServer_Fire_Implementation(SpawnLocation, SpawnRotation);
	
	if (WasBulletInMagazineBeforeShot)
	{
		bShotReady = false;
		World->GetTimerManager().SetTimer(ShotTimerHandle, this, &UAutomaticWeaponComponent::CooldownEnded, ShotDuration);
	}
}

void UAutomaticWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UAutomaticWeaponComponent, bShotReady, COND_OwnerOnly);
}

UAutomaticWeaponComponent::UAutomaticWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAutomaticWeaponComponent::StartShooting()
{
	if (bShotReady)
	{
		Super::StartShooting();
	}

	bTriggerDown = true;
}

void UAutomaticWeaponComponent::EndShooting()
{
	bTriggerDown = false;
}

