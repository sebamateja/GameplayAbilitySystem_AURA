#include "AbilitySystem/AsyncTask/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include <algorithm>

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
    UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
    WaitCooldownChange->ASC = AbilitySystemComponent;
    WaitCooldownChange->CooldownTag = InCooldownTag;

    if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
    {
        WaitCooldownChange->EndTask();
        return nullptr;
    }

    // To know when a cooldown has ended (CooldownTag has ben removed)
    AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
        WaitCooldownChange,
        &UWaitCooldownChange::CooldownTagChange
    );

    // To know when a cooldown effect has been applied
    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
        WaitCooldownChange,
        &UWaitCooldownChange::OnActiveEffectAdded
    );

    return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
    if (!IsValid(ASC)) return;
    ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

    SetReadyToDestroy();
    MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChange(const FGameplayTag InCooldownTag, int32 NewCount)
{
    if (NewCount == 0)
    {
        CooldownEnd.Broadcast(0.0f);
    }
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer AssetTags;
    SpecApplied.GetAllAssetTags(AssetTags);

    FGameplayTagContainer GrantedTags;
    SpecApplied.GetAllGrantedTags(GrantedTags);

    if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
    {
        FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());

        TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
        if (TimesRemaining.Num() > 0)
        {
            // Maybe I need to get highest time just in case, but should be only 1

            float TimeRemaining = TimesRemaining[0];
            // float TimeRemaining = *std::max_element(TimesRemaining.begin(), TimesRemaining.end());
            CooldownStart.Broadcast(TimeRemaining);
        }
    }
}