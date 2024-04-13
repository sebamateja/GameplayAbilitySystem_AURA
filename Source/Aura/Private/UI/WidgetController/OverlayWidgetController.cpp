#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"

#include "Player/AuraPlayerState.h"
#include "AuraGameplayTags.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    // No need to call Super::BroadcastInitialValues() because it is empty

    OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
    OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());

    OnManaChanged.Broadcast(GetAuraAS()->GetMana());
    OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
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

    GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
    GetAuraPS()->OnLevelChangedDelegate.AddLambda(
        [this](int32 NewLevel){
            OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
        });

    // ------------------------------------------------------------------------------
    // Vital Attributes
    // ------------------------------------------------------------------------------

    /** Health */
    BindToAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute(), &OnHealthChanged);
    /** Max Health */
    BindToAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute(), &OnMaxHealthChanged);
    /** Mana */
    BindToAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute(), &OnManaChanged);
    /** Max Mana */
    BindToAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute(), &OnMaxManaChanged);

    if (GetAuraASC())
    {
        GetAuraASC()->AbilityEquippedDelegate.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

        if (GetAuraASC()->bStartupAbilitiesGiven)
        {
            BroadcastAbilityInfo();
        }
        else
        {
            GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
        }

        GetAuraASC()->EffectAssetTags.AddLambda(
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

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
    const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;

    checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out AuraPlayerState blueprint"));

    const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
    const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

    if (Level <= MaxLevel && Level > 0)
    {
        const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
        const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

        const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
        const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

        const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

        OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
    }
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    // Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already-equipped spell
    FAuraAbilityInfo LastSlotInfo;
    LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
    LastSlotInfo.InputTag = PreviousSlot;
    LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
    AbilityInfoDelegate.Broadcast(LastSlotInfo);

    // Filling and broadcasting new slot
    FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
    Info.StatusTag = Status;
    Info.InputTag = Slot;
    AbilityInfoDelegate.Broadcast(Info);
}