#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "FireBlast.generated.h"

UCLASS()
class AURA_API UFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Custom|FireBlast")
	int32 NumFireBalls = 12;
};
