#include "AbilitySystem/AuraAttributeSet.h"

#include "Aura/AuraLogChannels.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "AuraAbilityTypes.h"
#include "Player/AuraPlayerController.h"
#include "Interaction/PlayerInterface.h"
#include "Interaction/CombatInterface.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UAuraAttributeSet::UAuraAttributeSet()
{
    // Constructor is to early to call the setter for attributes

    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    // Primary Attributes
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

    // Secondary Attributes
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);

    // Resistance Attributes - also works just like secondary
    TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

// Used to register variables to be replicated
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // It is registering health to be replicated.
    // COND_None = It is going to replicate without any conditions
    // REPNOTIFY_Always = If the value is set on the server, replicate it
    //    and on the client that value will be updated

    // Primary Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

    // Secondary Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

    // Vital Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

    // Resistance Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
}

// Recommended mostly for clamping the values
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
        // UE_LOG(LogTemp, Warning, TEXT("Health: %f"), NewValue);
    }
    if (Attribute == GetMaxHealthAttribute())
    {
        // UE_LOG(LogTemp, Warning, TEXT("MaxHealth: %f"), NewValue);
    }
    if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
        // UE_LOG(LogTemp, Warning, TEXT("Mana: %f"), NewValue);
    }
    if (Attribute == GetMaxManaAttribute())
    {
        // UE_LOG(LogTemp, Warning, TEXT("MaxMana: %f"), NewValue);
    }
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    // To fill up Mana and Health when leveling-up
    if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
    {
        SetHealth(GetMaxHealth());
        bTopOffHealth = false;
    }
    if (Attribute == GetMaxManaAttribute() && bTopOffMana)
    {
        SetMana(GetMaxMana());
        bTopOffMana = false;
    }
}

// It is executed after a GameplayEffect changes an attribute
// Happens only on the server
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FEffectProperties Props;
    SetEffectProperties(Data, Props);

    if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // UE_LOG(LogTemp, Warning, TEXT("Health from GetHealth(): %f"), GetHealth());
        // UE_LOG(LogTemp, Warning, TEXT("Magnitude: %f"), Data.EvaluatedData.Magnitude);
        SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
        // UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s, Health: %f"), *Props.TargetAvatarActor->GetName(), GetHealth());
    }
    if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
    }
    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        HandleIncomingDamage(Props);
    }
    if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
    {
        HandleIncomingXP(Props);
    }
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
    const float LocalIncomingDamage = GetIncomingDamage();
    SetIncomingDamage(0.0f);
    if (LocalIncomingDamage > 0.0f)
    {
        const float NewHealth = GetHealth() - LocalIncomingDamage;
        SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

        const bool bFatal = NewHealth <= 0.0f;
        if (bFatal)
        {
            ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
            if (CombatInterface)
            {
                CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
            }
            SendXPEvent(Props);
        }
        else
        {
            if (Props.TargetCharacter->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsBeingShocked(Props.TargetCharacter))
            {
                // We can specify a tag or a set of tags and say activate an ability if you have an ability with a specific tag
                FGameplayTagContainer TagContainer;
                TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
                Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
            }

        }
        const bool bBlock = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
        const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
        ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);

        // Debuff
        if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
        {
            HandleDebuff(Props);
        }
    }
}

void UAuraAttributeSet::HandleDebuff(const FEffectProperties& Props)
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    FGameplayEffectContextHandle EffectContextHandle = Props.SourceASC->MakeEffectContext();
    EffectContextHandle.AddSourceObject(Props.SourceAvatarActor);

    const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
    const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
    const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
    const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

    FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
    UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

    Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
    Effect->Period = DebuffFrequency;
    Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

    // Deprecated
    // Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.DamageTypesToDebuffs[DamageType]);
    const FGameplayTag DebuffTag = GameplayTags.DamageTypesToDebuffs[DamageType];
    FInheritedTagContainer TagContainer = FInheritedTagContainer();
    UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
    TagContainer.Added.AddTag(DebuffTag);
    TagContainer.CombinedTags.AddTag(DebuffTag);

    // To block rotation when Aura is stunned
    if (DebuffTag.MatchesTagExact(GameplayTags.Debuff_Stun))
    {
        TagContainer.Added.AddTag(GameplayTags.Player_Block_CursorTrace);
        TagContainer.CombinedTags.AddTag(GameplayTags.Player_Block_CursorTrace);

        TagContainer.Added.AddTag(GameplayTags.Player_Block_InputHeld);
        TagContainer.CombinedTags.AddTag(GameplayTags.Player_Block_InputHeld);

        TagContainer.Added.AddTag(GameplayTags.Player_Block_InputPressed);
        TagContainer.CombinedTags.AddTag(GameplayTags.Player_Block_InputPressed);

        TagContainer.Added.AddTag(GameplayTags.Player_Block_InputReleased);
        TagContainer.CombinedTags.AddTag(GameplayTags.Player_Block_InputReleased);
    }
    Component.SetAndApplyTargetTagChanges(TagContainer);

    Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
    Effect->StackLimitCount = 1;

    const int32 Index = Effect->Modifiers.Num();
    Effect->Modifiers.Add(FGameplayModifierInfo());
    FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];
    
    ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
    ModifierInfo.ModifierOp = EGameplayModOp::Additive;
    ModifierInfo.Attribute = UAuraAttributeSet::GetIncomingDamageAttribute();

    // Debuffs can have levels
    if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContextHandle, 1.0f))
    {
        FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());
        TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
        AuraEffectContext->SetDamageType(DebuffDamageType);

        Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
    }
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
    const float LocalIncomingXP = GetIncomingXP();
    SetIncomingXP(0.0f);

    // TODO: See if we should level up

    // SourceCharacter is the owner, since GA_ListenForEvents applies GE_EventBasedEffect,
    // adding to IncomingXP
    if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
    {
        const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
        const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

        const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
        const int32 NumberOfLevelUps = NewLevel - CurrentLevel;
        if (NumberOfLevelUps > 0)
        {
            const int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel);
            const int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel);
            
            IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumberOfLevelUps);
            IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
            IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

            bTopOffHealth = true;
            bTopOffMana = true;

            IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
        }

        IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
    }
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
    // Source = causer of the effect, Target = target of the effect (owner of this AS)

    Props.EffectContextHandle = Data.EffectSpec.GetContext();
    Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

    if (IsValid(Props.SourceASC) && 
        Props.SourceASC->AbilityActorInfo.IsValid() && 
        Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
    {
        Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
        Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
        if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
        {
            if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
            {
                Props.SourceController = Pawn->GetController();
            }
        }
        if (Props.SourceController)
        {
            Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
        }
    }

    if (Data.Target.AbilityActorInfo.IsValid() &&
        Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
        Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
        Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
        Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
    }
}

// if not self damage
// Called on the server - because we are calling this method in PostGameplayEffectExecute in condition
// for incomingDamage which is a meta attribute that is not replicated
void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
    if (Props.SourceCharacter != Props.TargetCharacter)
    {
        //GameplayStatics::GetPlayerController - it gives locally controlled PlayerController, but we need to provide index
        
        // We are getting the controller to the player that dealth the damage
        if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
        {
            AuraPC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
            return;
        }
        if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller))
        {
            AuraPC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
        }
    }
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
    if (Props.TargetCharacter->Implements<UCombatInterface>())
    {
        const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
        const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
        const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);
        
        const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

        FGameplayEventData Payload;
        Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
        Payload.EventMagnitude = XPReward;

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
    }
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}
