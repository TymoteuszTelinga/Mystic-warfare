#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.h"
#include "AutomaticWeaponComponent.generated.h"


UCLASS(ClassGroup=(Weapons), meta=(BlueprintSpawnableComponent))
class MYSTICWARFARE_API UAutomaticWeaponComponent : public UWeaponComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Gameplay)
	float ShotDuration = 0.5f;

	UPROPERTY(ReplicatedUsing=OnRep_bShotReady)
	bool bShotReady = true;

	UPROPERTY()
	bool bTriggerDown = false;
	UPROPERTY()
	FTimerHandle ShotTimerHandle;
	
	UFUNCTION()
	void OnRep_bShotReady();
	UFUNCTION()
	void CooldownEnded();
	
protected:
	virtual void RServer_Fire_Implementation(const FVector& SpawnLocation, const FRotator& SpawnRotation) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UAutomaticWeaponComponent();
	
	virtual void StartShooting() override;
	virtual void EndShooting() override;
};
