#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponComponent.generated.h"

class AActorPoolingManager;
class ABaseBullet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletsInMagazineChangedSignature, int32, NewValue);
DECLARE_MULTICAST_DELEGATE(FOnReloadFinishedSignature);

UENUM()
enum class EReloadDurationMode : uint8
{
	FIXED_DURATION					UMETA(DisplayName="Fixed Duration"),
	ON_RELOAD_ANIMATION_FINISHED	UMETA(DisplayName="On Reload Animation Finished")
};

UENUM()
enum class EBulletGetMethod : uint8
{
	SPAWN			UMETA(DisplayName="Spawn"),
	GET_FROM_POOL	UMETA(DisplayName="Get from Pool")
};

UCLASS(ClassGroup=(Weapons), meta=(BlueprintSpawnableComponent))
class MYSTICWARFARE_API UWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

	/** Bullet that will be spawned with each shot */
	UPROPERTY(EditAnywhere, Category=Gameplay)
	TSubclassOf<ABaseBullet> BulletClass;
	/** How many actual bullets will be spawned in one shot */
	UPROPERTY(EditAnywhere, Category=Gameplay)
	int32 PelletsInShot = 1;
	/** Max angle by which shoot rotation can be randomly offseted */
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=0.0))
	float DispersionAngle = 2.5f;
	/** Weight in range 0.0-1.0. Determinate randomness of dispersion. 0.0 is uniform random, 1.0 is normal distribution,
	 * everything between is blend between them */
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=0.0, UIMax=1.0))
	float DispersionWeight = 0.75f;
	/** Max ammo per clip */
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=1))
	int32 MagazineSize = 7;
	/** Determinate when reload ends */
	UPROPERTY(EditAnywhere, Category=Gameplay)
	EReloadDurationMode ReloadDurationMode = EReloadDurationMode::FIXED_DURATION;
	/** Used in Fixed Duration Mode and as a fallback duration if animation cannot be played */
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=0.0f))
	float FixedReloadDuration = 1.0f;
	/** How many bullets are restored by reload. Values below 1 mean to restore full magazine.
	 * If value here is greater then MagazineSize it gets clamped */
	UPROPERTY(EditAnywhere, Category=Gameplay)
	int32 ReloadRestoredBulletsNum = -1;
	/** Used mostly in shotguns. Determinate behavior of weapon when reload does not restore full magazine */
	UPROPERTY(EditAnywhere, Category=Gameplay)
	bool bStartAnotherReloadIfMagazineIsNotFull = true;
	/** Determinate method of getting bullet for shooting */
	UPROPERTY(EditAnywhere, Category=Gameplay)
	EBulletGetMethod BulletGetMethod = EBulletGetMethod::GET_FROM_POOL;
	/** Can reload be canceled, for example by shooting */
	UPROPERTY(EditAnywhere, Category=Gameplay)
	bool bBreakableReload = false;
	UPROPERTY(EditAnywhere, Category=Animation)
	float FireAimPlayRate = 1.0f;
	UPROPERTY(EditAnywhere, Category=Animation)
	TObjectPtr<UAnimMontage> FireAnimation;
	UPROPERTY(EditAnywhere, Category=Animation)
	float ReloadAnimPlayRate = 1.0f;
	UPROPERTY(EditAnywhere, Category=Animation)
	TObjectPtr<UAnimMontage> ReloadAnimation;
	UPROPERTY(EditAnywhere, Category=Sounds)
	TObjectPtr<USoundBase> FireSound;
	UPROPERTY(EditAnywhere, Category=Sounds)
	TObjectPtr<USoundBase> EmptyMagSound;
	UPROPERTY(EditAnywhere, Category=Sounds)
	TObjectPtr<USoundBase> ReloadSound;

	UPROPERTY(ReplicatedUsing=OnRep_BulletsInMagazine)
	int32 BulletsInMagazine;
	UPROPERTY(Replicated)
	bool bDuringReload;
	
	UPROPERTY()
	FTimerHandle FixedReloadTimerHandle;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent = nullptr;
	UPROPERTY()
	TObjectPtr<AActorPoolingManager> PoolingManager = nullptr;
	UPROPERTY()
	bool bSkeletalMeshProvided = false;
	
	UFUNCTION()
	void OnRep_BulletsInMagazine();
	
	void FinishReload();
	void CancelReload();
	ABaseBullet* GetBulletInstance();
	ABaseBullet* GetBulletFromPool();
	ABaseBullet* SpawnBullet();
	float GetNormalDistributionRand(float Weight);

protected:
	FOnReloadFinishedSignature OnReloadFinishedDelegate;
	
	UFUNCTION(Server, Reliable)
	virtual void RServer_Fire(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	UFUNCTION(Server, Reliable)
	void RServer_Reload();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayFireEffects();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayEmptyMagEffects();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayReloadEffects();

	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool TryGetReadyForShot();
	void PlayFireEffects() const;
	FORCEINLINE USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }
	FORCEINLINE bool IsDuringReload() const { return bDuringReload; }
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnBulletsInMagazineChangedSignature OnBulletsInMagazineChangedDelegate;
	
	UWeaponComponent();

	UFUNCTION(BlueprintCallable)
	virtual void StartShooting();
	UFUNCTION(BlueprintCallable)
	virtual void EndShooting();
	UFUNCTION(BlueprintCallable)
	virtual void Reload();
	
	UFUNCTION(BlueprintCallable)
	void SetSkeletalMeshForAnimation(USkeletalMeshComponent* SMComponent);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetMagazineSize() const { return MagazineSize; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetBulletsInMagazine() const { return BulletsInMagazine; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsSkeletalMeshProvided() const { return bSkeletalMeshProvided; }
};
