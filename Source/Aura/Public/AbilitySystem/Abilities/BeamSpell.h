#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "BeamSpell.generated.h"

UCLASS()
class AURA_API UBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);

	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Beam")
	int32 MaxNumShockTargets = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom|Beam")
	float DamageDeltaTime = 0.1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom|Beam")
	float MinSpellTime = 0.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom|Beam")
	bool TargetDead = false;
};
