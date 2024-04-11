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

	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

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
};
