#include "WeaponsSystem/HitScanBullet.h"

#include "Kismet/GameplayStatics.h"
#include "MysticWarfare/MysticWarfare.h"
#include "Utils/ActorPoolingManager.h"
#include "WeaponsSystem/Damageable.h"
#include "WeaponsSystem/VisualEffect.h"


void AHitScanBullet::Multicast_PlayHitSoundAtLocation_Implementation(const FVector& Location)
{
	const UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("AHitScanBullet::Multicast_PlayHitSoundAtLocation_Implementation: Cannot access world"));
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(World, WallHitSound, Location);
}

AHitScanBullet::AHitScanBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AHitScanBullet::Fire_Implementation(const FRotator& Direction, const AActor* Shooter)
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("AHitScanBullet::Fire_Implementation: Cannot access world"));
		return;
	}

	SetActorRotation(Direction);
	const FVector Start = GetActorLocation();
	const FVector End = Start + Direction.RotateVector(FVector::ForwardVector) * TraceLength;
	FHitResult Result;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Shooter);
	
	World->LineTraceSingleByChannel(Result, Start, End, ECC_Pawn, QueryParams);

	if (AActor* HitActor = Result.GetActor())
	{
		if (HitActor->Implements<UDamageable>())
		{
			const float FinalDamage = FMath::RandRange(Damage * (1.0f - RandomDamageDeviation), Damage * (1.0f + RandomDamageDeviation));
			IDamageable::Execute_ApplyDamage(HitActor, FinalDamage);
		}
		else
		{
			if (WallHitVisualEffect)
			{
				AActor* WallHitVFX;
				if (bUsesPoolingManager)
				{
					WallHitVFX = OwnPoolingManager->GetActorFromPool(WallHitVisualEffect);
				}
				else
				{
					WallHitVFX = World->SpawnActor(WallHitVisualEffect);
				}
				WallHitVFX->SetActorLocation(Result.Location);
				IVisualEffect::Execute_Play(WallHitVFX, Result.ImpactNormal);
			}

			if (WallHitSound != nullptr)
			{
				Multicast_PlayHitSoundAtLocation(Result.Location);
			}
		}
	}

	if (bUsesPoolingManager)
	{
		OwnPoolingManager->ReturnActorToPool(this);
	}
	else
	{
		Destroy();
	}
}

void AHitScanBullet::Init_Implementation(AActorPoolingManager* OwnAPM)
{
	bUsesPoolingManager = true;
	OwnPoolingManager = OwnAPM;

	if (!OwnPoolingManager->IsClassRegistered(WallHitVisualEffect))
	{
		if (WallHitVisualEffect)
		{
			OwnPoolingManager->RegisterActors(WallHitVisualEffect, PreCreateWallHitVfxNum, ENoInstancesHandlingMethod::SPAWN_NEW_INSTANCE_AND_LOG_WARNING);
		}
	}
}




