#pragma once

#include "CoreMinimal.h"
#include "BaseBullet.h"
#include "Utils/Poolable.h"
#include "HitScanBullet.generated.h"

class UVisualEffect;
class IVisualEffect;

UCLASS()
class MYSTICWARFARE_API AHitScanBullet : public ABaseBullet, public IPoolable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Gameplay)
	float Damage = 20.0f;
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=0.0))
	float RandomDamageDeviation = 0.2f;
	UPROPERTY(EditAnywhere, Category=Gameplay)
	float TraceLength = 10000.0;
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(MustImplement="VisualEffect"))
	TSubclassOf<AActor> WallHitVisualEffect;
	UPROPERTY(EditAnywhere, Category=Gameplay)
	int32 PreCreateWallHitVfxNum = 100;
	UPROPERTY(EditAnywhere, Category=Sounds)
	TObjectPtr<USoundBase> WallHitSound;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY()
	TObjectPtr<AActorPoolingManager> OwnPoolingManager;
	UPROPERTY()
	bool bUsesPoolingManager = false;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitSoundAtLocation(const FVector& Location);
	
public:
	AHitScanBullet();

	virtual void Fire_Implementation(const FRotator& Direction, const AActor* Shooter) override;

	// Poolable Interface
	virtual void Init_Implementation(AActorPoolingManager* OwnAPM) override;
};
