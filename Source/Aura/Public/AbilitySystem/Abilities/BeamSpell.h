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
protected:

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
};
