#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "SummonAbility.generated.h"

UCLASS()
class AURA_API USummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure, Category = "Summoning")
	TSubclassOf<APawn> GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Summoning")
	int32 NumberOfMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Summoning")
	float MinSpawnDistance = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Summoning")
	float MaxSpawnDistance = 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Summoning")
	float SpawnSpread = 90.0f;
};
