#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::Client_EffectApplied);

    //Example how we can access GameplayTags from FAuraGameplayTag singleton
    // const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
    // GameplayTags.Attributes_Secondary_Armor.ToString();
}

void UAuraAbilitySystemComponent::Client_EffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, 
                                                const FGameplayEffectSpec& EffectSpec, 
                                                FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    
    EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    // FGameplayAbilitySpec has TagContainer specifically for using it in runtime
    // to change InputTags
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1.0);
        if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
            GiveAbility(AbilitySpec); // tu AbilitySpec może być const
        }
        //GiveAbilityAndActivateOnce(AbilitySpec); // tu AbilitySpec nie może być const
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputPressed(AbilitySpec);
            if (!AbilitySpec.IsActive())
            {
                TryActivateAbility(AbilitySpec.Handle);
            }
        }
    }
}

// In released case we do not always want to stop the activated ability
// in most of the cases we just want to say to the ability that it is not pressed
// anymore
// In Ability itself we can override method InputPressed and InputReleased to
// do whatever we need in case of those events
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputReleased(AbilitySpec);
        }
    }
}