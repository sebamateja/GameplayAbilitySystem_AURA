#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "OverlayWidgetController.generated.h"

class UAuraAbilitySystemComponent;
struct FOnAttributeChangeData;
class UAuraUserWidget;
class UAbilityInfo;
struct FAuraAbilityInfo;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);

// Blueprintable - blueprint can be created from it
// BlueprintType - this can be used in blueprint graphs
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// <---- Health begin
	UPROPERTY(BlueprintAssignable, Category="Custom|GAS|Attribute")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Custom|GAS|Attribute")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	// <---- Health end

	// <---- Mana begin
	UPROPERTY(BlueprintAssignable, Category="Custom|GAS|Attribute")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category="Custom|GAS|Attribute")
	FOnAttributeChangedSignature OnMaxManaChanged;
	// <---- Mana end

	UPROPERTY(BlueprintAssignable, Category="Custom|GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category="Custom|GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Custom|GAS|Level")
	FOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	void BindToAttributeValueChangeDelegate(const FGameplayAttribute& Attribute, FOnAttributeChangedSignature* AttributeChangeDelegate);

	void OnXPChanged(int32 NewXP);

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template<typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
