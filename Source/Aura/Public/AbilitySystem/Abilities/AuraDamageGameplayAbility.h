#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	FScalableFloat Damage;

	/** Debuffs begins*/
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	float DebuffChance = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	float DebuffDamage = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	float DebuffFrequency = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	float DebuffDuration = 5.0f;
	/** Debuffs ends*/

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	float DeathImpulseMagnitude = 1000.0f;

	/** Radial damage begins*/
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Damage")
	float RadialDamageInnerRadius = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Damage")
	float RadialDamageOuterRadius = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Damage")
	FVector RadialDamageOrigin = FVector::ZeroVector;
	/** Radial damage ends*/
};
