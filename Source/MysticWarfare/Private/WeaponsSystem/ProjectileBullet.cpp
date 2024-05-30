#include "WeaponsSystem/ProjectileBullet.h"

#include "Kismet/GameplayStatics.h"
#include "MysticWarfare/MysticWarfare.h"
#include "Utils/ActorPoolingManager.h"
#include "WeaponsSystem/Damageable.h"
#include "WeaponsSystem/VisualEffect.h"


void AProjectileBullet::FinishShot()
{
	if (!bFired)
	{
		return;
	}

	const UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("AProjectileBullet::OnReturnedToPool_Implementation: Cannot access world"));
		return;
	}
	
	if (bUsesPoolingManager)
	{
		OwnPoolingManager->ReturnActorToPool(this);
		bFired = false;
	}
	else
	{
		Destroy();
	}
	
	if (FlyingTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(FlyingTimerHandle);
		FlyingTimerHandle.Invalidate();
	}
}

void AProjectileBullet::Multicast_PlayHitSoundAtLocation_Implementation(const FVector& Location)
{
	const UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("AProjectileBullet::Multicast_PlayHitSoundAtLocation_Implementation: Cannot access world"));
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(World, WallHitSound, Location);
}

void AProjectileBullet::OnBulletMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bFired)
	{
		return;
	}
	
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("AProjectileBullet::OnBulletMeshHit: Cannot access world"));
		return;
	}
	
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
		WallHitVFX->SetActorLocation(Hit.Location);
		IVisualEffect::Execute_Play(WallHitVFX, Hit.ImpactNormal);
	}

	if (WallHitSound != nullptr)
	{
		Multicast_PlayHitSoundAtLocation(Hit.Location);
	}

	FinishShot();
}

void AProjectileBullet::OnBulletMeshOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bFired)
	{
		return;
	}
	
	if (OtherActor->Implements<UDamageable>())
	{
		if (OtherActor == CurrentShooter)
		{
			return;
		}

		const float FinalDamage = FMath::RandRange(Damage * (1.0f - RandomDamageDeviation), Damage * (1.0f + RandomDamageDeviation));
		IDamageable::Execute_ApplyDamage(OtherActor, FinalDamage);
		FinishShot();
	}
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	bFired = false;

	BulletMesh->OnComponentHit.AddUniqueDynamic(this, &AProjectileBullet::OnBulletMeshHit);
	BulletMesh->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileBullet::OnBulletMeshOverlap);
}

void AProjectileBullet::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetActorLocation(Speed * GetActorForwardVector() * DeltaSeconds + GetActorLocation(), true);
}

AProjectileBullet::AProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh"));

	SetRootComponent(BulletMesh);
	BulletMesh->SetCollisionProfileName(TEXT("Projectile"));
	BulletMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AProjectileBullet::Fire_Implementation(const FRotator& Direction, const AActor* Shooter)
{
	const UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogMWWeaponsSystem, Error, TEXT("AProjectileBullet::Fire_Implementation: Cannot access world"));
		return;
	}
	
	World->GetTimerManager().SetTimer(FlyingTimerHandle, this, &AProjectileBullet::FinishShot, MaxFiredTime, false);

	SetActorRotation(Direction);
	SetActorTickEnabled(true);
	bFired = true;
	CurrentShooter = Shooter;
}

void AProjectileBullet::Init_Implementation(AActorPoolingManager* OwnAPM)
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

