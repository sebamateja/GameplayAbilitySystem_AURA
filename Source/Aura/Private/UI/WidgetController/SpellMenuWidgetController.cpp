#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
    BroadcastAbilityInfo();

    SpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
    GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
    {
        if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
        {
            SelectedAbility.Status = StatusTag;

            bool bEnableSpendPoints = false;
            bool bEnableEquip = false;
            ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

            FString Description;
            FString NextLevelDescription;
            GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);

            SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
        }

        if (AbilityInfo)
        {
            FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
            Info.StatusTag = StatusTag;
            AbilityInfoDelegate.Broadcast(Info);
        }
    });

    GetAuraASC()->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

    GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
		{
			SpellPointsChanged.Broadcast(SpellPoints);
            CurrentSpellPoints = SpellPoints;

            bool bEnableSpendPoints = false;
            bool bEnableEquip = false;
            ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

            FString Description;
            FString NextLevelDescription;
            GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);

            SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
		}
	);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
    if (GetAuraASC())
    {
        GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
    }
}

void USpellMenuWidgetController::SelectAbility(UAuraUserWidget* AbilityButton, const FGameplayTag& AbilityTag)
{
    // Delegate for visual effect of selecting the SpellGlobe
    AbilitySelectedDelegate.Broadcast(AbilityButton);

    if (bWaitingForEquipSelection)
    {
        const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
        StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
        bWaitingForEquipSelection = false;
    }

    // Logic to check whether SpellPoints button and Equip button should be enabled
    const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
    const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
    FGameplayTag AbilityStatus;

    const bool bTagValid = AbilityTag.IsValid();
    const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
    const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
    const bool bSpecValid = AbilitySpec != nullptr;

    if (!bTagValid || bTagNone || !bSpecValid)
    {
        AbilityStatus = GameplayTags.Abilities_Status_Locked;
    }
    else
    {
        AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
    }

    SelectedAbility.Ability = AbilityTag;
    SelectedAbility.Status = AbilityStatus;

    bool bEnableSpendPoints = false;
    bool bEnableEquip = false;
    ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);

    FString Description;
    FString NextLevelDescription;
    GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);

    SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}

void USpellMenuWidgetController::DeselectAbility()
{
    if (bWaitingForEquipSelection)
    {
        const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
        StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
        bWaitingForEquipSelection = false;
    }

    SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
    SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;

    SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
    const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;

    WaitForEquipDelegate.Broadcast(AbilityType);
    bWaitingForEquipSelection = true;

    const FGameplayTag SelectedAbilityStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
    if (SelectedAbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
    {
        SelectedAbilitySlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
    }
}

// Clicking on bottom spell row equipped when wanting to equip some spell to slot
void USpellMenuWidgetController::SpellRowEquipGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
    if (!bWaitingForEquipSelection) return;
    // Check selected ability against the slot's ability type. 
    // (dont equip an offensive spell in a passive slot and vice versa)
    const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
    if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;
    
    GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
    bWaitingForEquipSelection = false;

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

    // To stop playing animations in UI
    StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
    
    SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
    DeselectAbility();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, const int32 SpellPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton)
{
    const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

    bShouldEnableSpendPointsButton = false;
    if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
    {
        bShouldEnableEquipButton = true;
        if (SpellPoints > 0)
        {
            bShouldEnableSpendPointsButton = true;
        }
    }
    else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
    {
        bShouldEnableEquipButton = false;
        if (SpellPoints > 0)
        {
            bShouldEnableSpendPointsButton = true;
        }
    }
    else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
    {
        bShouldEnableEquipButton = true;
        if (SpellPoints > 0)
        {
            bShouldEnableSpendPointsButton = true;
        }
    }
    else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
    {
        bShouldEnableEquipButton = false;
        bShouldEnableSpendPointsButton = false;
    }
}