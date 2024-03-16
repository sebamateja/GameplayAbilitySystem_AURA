#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilitySelectedSignature, UAuraUserWidget*, AbilityButton);

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChanged;

	/** Selecting Ability Button begins*/
	UPROPERTY(BlueprintAssignable)
    FAbilitySelectedSignature AbilitySelectedDelegate;

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells Menu")
    void SelectAbility(UAuraUserWidget* AbilityButton);
	/** Selecting Ability Button ends*/
};
