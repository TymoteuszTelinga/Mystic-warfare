#include "MysticWarfare/Public/WeaponsSystem/WeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "MysticWarfare/MysticWarfare.h"
#include "MysticWarfare/Public/WeaponsSystem/BaseBullet.h"
#include "Net/UnrealNetwork.h"
#include "Utils/ActorPoolingManager.h"


void UWeaponComponent::Multicast_PlayFireEffects_Implementation()
{
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (OwnerPawn->IsLocallyControlled())
		{
			return;
		}
	}
	
	PlayFireEffects();
}

void UWeaponComponent::RServer_Fire_Implementation(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (BulletsInMagazine < 1)
	{
		if (EmptyMagSound != nullptr)
		{
			Multicast_PlayEmptyMagEffects();
		}
		
		return;
	}
	
	if (BulletClass == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Warning, TEXT("UWeaponComponent::RServer_Fire_Implementation: Bullet class hasn't been set"));
		return;
	}

	for (int32 Index = 0; Index < PelletsInShot; Index++)
	{
		ABaseBullet* Bullet = GetBulletInstance();
		Bullet->SetActorLocation(SpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);

		FVector2d Angle(0.0f, 1.0f);
		Angle = Angle.GetRotated(FMath::RandRange(0.0f, 360.0f));
		const float RandomizedDispersionAngle = DispersionAngle * (GetNormalDistributionRand(DispersionWeight) * 2.0f - 1.0f);
		const FRotator DispersionOffset(Angle.X * RandomizedDispersionAngle, Angle.Y * RandomizedDispersionAngle, 0.0f);
	
		Bullet->Fire(SpawnRotation + DispersionOffset, GetOwner());
	}
	
	Multicast_PlayFireEffects();
	BulletsInMagazine -= 1;
	OnRep_BulletsInMagazine();
}

void UWeaponComponent::Multicast_PlayEmptyMagEffects_Implementation()
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Warning, TEXT("UWeaponComponent::Multicast_PlayEmptyMagEffects_Implementation: Cannot access World"));
		return;
	}

	if (EmptyMagSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(World, EmptyMagSound, GetComponentLocation());
	}
}

void UWeaponComponent::RServer_Reload_Implementation()
{
	if (BulletsInMagazine >= MagazineSize || bDuringReload)
	{
		return;
	}

	bDuringReload = true;

	if (ReloadDurationMode == EReloadDurationMode::ON_RELOAD_ANIMATION_FINISHED && !bSkeletalMeshProvided)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("UWeaponComponent::RServer_Reload_Implementation: Reload iniciated while skeletal mesh was not provided. Fallback to Fixed Duration"));
		ReloadDurationMode = EReloadDurationMode::FIXED_DURATION;
	}

	if (ReloadDurationMode == EReloadDurationMode::FIXED_DURATION)
	{
		const UWorld* World = GetWorld();

		if (World == nullptr)
		{
			UE_LOG(LogMWWeaponsSystem, Error, TEXT("UWeaponComponent::RServer_Reload_Implementation: Cannot access world"));
			return;
		}

		World->GetTimerManager().SetTimer(FixedReloadTimerHandle, this, &UWeaponComponent::FinishReload, FixedReloadDuration, false);
	}

	Multicast_PlayReloadEffects();
}

void UWeaponComponent::Multicast_PlayReloadEffects_Implementation()
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("UWeaponComponent::Multicast_PlayReloadEffects_Implementation: Cannot access World"));
		return;
	}

	if (ReloadSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(World, ReloadSound, GetComponentLocation());
	}

	if (SkeletalMeshComponent != nullptr && ReloadAnimation != nullptr)
	{
		if (UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance())
		{
			AnimInstance->Montage_Play(ReloadAnimation, ReloadAnimPlayRate);
		}
	}
}

void UWeaponComponent::OnRep_BulletsInMagazine()
{
	OnBulletsInMagazineChangedDelegate.Broadcast(BulletsInMagazine);
}

void UWeaponComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == ReloadAnimation && !bInterrupted)
	{
		FinishReload();
	}
}

void UWeaponComponent::PlayFireEffects() const
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Warning, TEXT("UWeaponComponent::PlayFireEffects: Cannot access World"));
		return;
	}

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(World, FireSound, GetComponentLocation());
	}

	if (SkeletalMeshComponent != nullptr && FireAnimation != nullptr)
	{
		if (UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance())
		{
			AnimInstance->Montage_Play(FireAnimation, FireAimPlayRate);
		}
	}
}

void UWeaponComponent::FinishReload()
{
	if (GetOwner()->HasAuthority() && bDuringReload)
	{
		if (ReloadRestoredBulletsNum < 1 || ReloadRestoredBulletsNum > MagazineSize)
		{
			BulletsInMagazine = MagazineSize;
		}
		else
		{
			BulletsInMagazine += ReloadRestoredBulletsNum;
			BulletsInMagazine = FMath::Min(BulletsInMagazine, MagazineSize);
		}
		OnRep_BulletsInMagazine();
		bDuringReload = false;

		if (bStartAnotherReloadIfMagazineIsNotFull && BulletsInMagazine < MagazineSize)
		{
			RServer_Reload();
		}
		else
		{
			OnReloadFinishedDelegate.Broadcast();
		}
	}
}

void UWeaponComponent::CancelReload()
{
	bDuringReload = false;
	OnReloadFinishedDelegate.Broadcast();
}

ABaseBullet* UWeaponComponent::GetBulletInstance()
{
	return BulletGetMethod == EBulletGetMethod::SPAWN ? SpawnBullet() : GetBulletFromPool();
}

ABaseBullet* UWeaponComponent::GetBulletFromPool()
{
	check(PoolingManager != nullptr);
	return CastChecked<ABaseBullet>(PoolingManager->GetActorFromPool(BulletClass));
}

ABaseBullet* UWeaponComponent::SpawnBullet()
{
	UWorld* World = GetWorld();
	check(World != nullptr);
	
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.bNoFail = true;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector SpawnLocation = GetComponentLocation();
	const FRotator SpawnRotation = GetComponentRotation();
	
	return CastChecked<ABaseBullet>(World->SpawnActor(BulletClass, &SpawnLocation, &SpawnRotation));
}

float UWeaponComponent::GetNormalDistributionRand(float Weight)
{
	float Result = FMath::RandRange(0.0f, 1.0f);

	if (Weight <= 0.0)
	{
		return Result;
	}

	const float InitialRoll = Result;
	Result += FMath::RandRange(0.0f, 1.0f);
	Result += FMath::RandRange(0.0f, 1.0f);
	Result /= 3.0f;
	Result = FMath::Clamp(Result, 0.0f, 1.0f);

	Result = FMath::Lerp(InitialRoll, Result, Weight);
	
	return Result;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BulletsInMagazine = MagazineSize;

	if (GetOwner()->HasAuthority())
	{
		if (BulletGetMethod == EBulletGetMethod::GET_FROM_POOL)
		{
			AActor* ReturnedActor = UGameplayStatics::GetActorOfClass(this, AActorPoolingManager::StaticClass());
			if (AActorPoolingManager* Manager = Cast<AActorPoolingManager>(ReturnedActor))
			{
				PoolingManager = Manager;
				PoolingManager->RegisterActors(BulletClass, MagazineSize * PelletsInShot);
			}
			else
			{
				UE_LOG(LogMWWeaponsSystem, Error, TEXT("UWeaponComponent::BeginPlay: Couldn't access Pool actor; falling back to spawning bullets"));
				BulletGetMethod = EBulletGetMethod::SPAWN;
			}
		}
	}
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UWeaponComponent, BulletsInMagazine, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponComponent, bDuringReload, COND_OwnerOnly);
}

bool UWeaponComponent::TryGetReadyForShot()
{
	if (bDuringReload)
	{
		if (bBreakableReload)
		{
			CancelReload();
		}
		else
		{
			return false;
		}
	}

	return true;
}

UWeaponComponent::UWeaponComponent()
{
	bDuringReload = false;
	
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}

void UWeaponComponent::StartShooting()
{
	if (!TryGetReadyForShot())
	{
		return;
	}
	
	if (BulletsInMagazine > 0)
	{
		PlayFireEffects();
	}
	RServer_Fire(GetComponentLocation(), GetComponentRotation());
}

void UWeaponComponent::EndShooting()
{
	// Does nothing in this class
}

void UWeaponComponent::Reload()
{
	if (BulletsInMagazine < MagazineSize && !bDuringReload)
	{
		RServer_Reload();
	}
}

void UWeaponComponent::SetSkeletalMeshForAnimation(USkeletalMeshComponent* SMComponent)
{
	SkeletalMeshComponent = SMComponent;
	bSkeletalMeshProvided = true;
	
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	if (ReloadDurationMode == EReloadDurationMode::ON_RELOAD_ANIMATION_FINISHED)
	{
		if (UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &UWeaponComponent::OnMontageEnded);
		}
		else
		{
			UE_LOG(LogMWWeaponsSystem, Error, TEXT("UWeaponComponent::SetSkeletalMeshForAnimation: Couldn't get AnimInstance, fallback to fixed dureation"));
			ReloadDurationMode = EReloadDurationMode::FIXED_DURATION;
		}
	}
}
