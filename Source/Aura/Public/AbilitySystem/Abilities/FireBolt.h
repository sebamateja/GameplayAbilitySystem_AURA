#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "FireBolt.generated.h"

UCLASS()
class AURA_API UFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Firebolt")
	float ProjectileSpread = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Firebolt")
	int32 MaxNumProjectiles = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Firebolt")
	float HomingAccelerationMin = 1600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Firebolt")
	float HomingAccelerationMax = 3200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Firebolt")
	bool bLaunchHomingProjectiles = true;
};
