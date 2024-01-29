#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    // No need to call Super::BroadcastInitialValues() because it is empty

    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

    OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
    OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindToAttributeValueChangeDelegate(const FGameplayAttribute& Attribute, FOnAttributeChangedSignature* AttributeChangeDelegate)
{
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
        [AttributeChangeDelegate](const FOnAttributeChangeData& Data)
        {
            AttributeChangeDelegate->Broadcast(Data.NewValue);
        }
    );
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    // No need to call Super::BindCallbacksToDependencies() because it is empty

    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

    // ------------------------------------------------------------------------------
    // Vital Attributes
    // ------------------------------------------------------------------------------

    /** Health */
    BindToAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute(), &OnHealthChanged);
    /** Max Health */
    BindToAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute(), &OnMaxHealthChanged);
    /** Mana */
    BindToAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute(), &OnManaChanged);
    /** Max Mana */
    BindToAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute(), &OnMaxManaChanged);

    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
    {
        if (AuraASC->bStartupAbilitiesGiven)
        {
            OnInitializeStartupAbilities(AuraASC);
        }
        else
        {
            AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
        }

        AuraASC->EffectAssetTags.AddLambda(
            [this](const FGameplayTagContainer& AssetTags) {
                for(const FGameplayTag& Tag : AssetTags)
                {
                    // const FString Msg = FString::Printf(TEXT("OverlayWidgetController::GE Tag: %s"), *Tag.ToString()); // Tag.GetTagName()
                    // GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Blue, Msg);

                    FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
                    if (Tag.MatchesTag(MessageTag))
                    {
                        FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
                        MessageWidgetRowDelegate.Broadcast(*Row);
                    }
                }
            }
        );
    }
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC)
{
    if (!AuraASC->bStartupAbilitiesGiven) return;

    FForEachAbility BroadcastDelegate;
    BroadcastDelegate.BindLambda([this, AuraASC](const FGameplayAbilitySpec& AbilitySpec){
        FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraASC->GetAbilityTagFromSpec(AbilitySpec));
        Info.InputTag = AuraASC->GetInputTagFromSpec(AbilitySpec);
        AbilityInfoDelegate.Broadcast(Info);
    });

    AuraASC->ForEachAbility(BroadcastDelegate);
}