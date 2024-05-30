#pragma once

#include "CoreMinimal.h"
#include "BaseBullet.h"
#include "Utils/Poolable.h"
#include "ProjectileBullet.generated.h"

class UPhysicsHandleComponent;

UCLASS()
class MYSTICWARFARE_API AProjectileBullet : public ABaseBullet, public IPoolable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=0.0f))
	float MaxFiredTime = 10.0f;
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=0.0f));
	float Speed = 1000.0f;
	UPROPERTY(EditAnywhere, Category=Gameplay)
	float Damage = 20.0f;
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=0.0))
	float RandomDamageDeviation = 0.2f;
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(MustImplement="VisualEffect"))
	TSubclassOf<AActor> WallHitVisualEffect;
	UPROPERTY(EditAnywhere, Category=Gameplay)
	int32 PreCreateWallHitVfxNum = 100;
	UPROPERTY(EditAnywhere, Category=Sounds)
	TObjectPtr<USoundBase> WallHitSound;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BulletMesh;

	UPROPERTY()
	TObjectPtr<const AActor> CurrentShooter;
	UPROPERTY()
	bool bFired = false;
	UPROPERTY()
	FTimerHandle FlyingTimerHandle;
	UPROPERTY()
	TObjectPtr<AActorPoolingManager> OwnPoolingManager;
	UPROPERTY()
	bool bUsesPoolingManager = false;
	
	void FinishShot();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitSoundAtLocation(const FVector& Location);
	UFUNCTION()
	void OnBulletMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnBulletMeshOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	AProjectileBullet();
	
	virtual void Fire_Implementation(const FRotator& Direction, const AActor* Shooter) override;

	// Poolable Interface
	virtual void Init_Implementation(AActorPoolingManager* OwnAPM) override;
};
