#include "AbilitySystem/AuraAbilitySystemComponent.h"


void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, 
                                                const FGameplayEffectSpec& EffectSpec, 
                                                FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    for(const FGameplayTag& Tag : TagContainer)
    {
        // TODO: Broadcast the tag to the WidgetController
        const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString()); // Tag.GetTagName()
        GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Blue, Msg);
    }
}