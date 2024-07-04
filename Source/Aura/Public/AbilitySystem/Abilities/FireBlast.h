#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "FireBlast.generated.h"

class AFireBall;

UCLASS()
class AURA_API UFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	TArray<AFireBall*> SpawnFireBalls();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Custom|FireBlast")
	int32 NumFireBalls = 12;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFireBall> FireBallClass;
};
