#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.h"
#include "InsertActionWeaponComponent.generated.h"

UENUM()
enum class EInsertDurationMode : uint8
{
	FIXED_DURATION					UMETA(DisplayName="Fixed Duration"),
	ON_RELOAD_ANIMATION_FINISHED	UMETA(DisplayName="On Reload Animation Finished")
};

UCLASS(ClassGroup=(Weapons), meta=(BlueprintSpawnableComponent))
class MYSTICWARFARE_API UInsertActionWeaponComponent : public UWeaponComponent
{
	GENERATED_BODY()

	/** Determinate when insert ends */
	UPROPERTY(EditAnywhere, Category=Gameplay)
	EInsertDurationMode InsertDurationMode = EInsertDurationMode::FIXED_DURATION;
	/** Used in Fixed Duration Mode and as a fallback duration if animation cannot be played */
	UPROPERTY(EditAnywhere, Category=Gameplay, meta=(UIMin=0.0f))
	float FixedInsertDuration = 1.0f;
	UPROPERTY(EditAnywhere, Category=Animation)
	float InsertAnimPlayRate = 1.0f;
	UPROPERTY(EditAnywhere, Category=Animation)
	TObjectPtr<UAnimMontage> InsertAnimation;
	UPROPERTY(EditAnywhere, Category=Sounds)
	TObjectPtr<USoundBase> InsertSound;

	UPROPERTY(Replicated)
	bool bBulletInserted = true;

	UPROPERTY()
	FTimerHandle FixedInsertTimerHandle;

	UFUNCTION()
	void OnReloadFishedDelegateHandler();
	
	void TryInsertBullet();
	void FinishInsert();
	
protected:
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayInsertEffects();

	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	virtual void BeginPlay() override;
	
	virtual void RServer_Fire_Implementation(const FVector& SpawnLocation, const FRotator& SpawnRotation) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UInsertActionWeaponComponent();

	virtual void StartShooting() override;
};
