#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"

#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
    // Target
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

    // Source
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

    AuraDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
    }
};

static const AuraDamageStatics& DamageStatics()
{
    static AuraDamageStatics DStatics;
    return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);

    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

    TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
    TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    int32 TargetLevel = 1;
    int32 SourceLevel = 1;
    if (SourceAvatar->Implements<UCombatInterface>())
    {
        SourceLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
    }
    if (TargetAvatar->Implements<UCombatInterface>())
    {
        TargetLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
    }

    const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);

    
    // Gather tags from source and target
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // These are the parameters that we have to pass in to a specific
    // function in order to capture the attribute we are interested in (in our case Vigor)
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;



    

    // Debuff
    DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);
    




    // Get Damage Set by Caller Magnitude
    float Damage = 0.0f;
    for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToResistances)
    {
        const FGameplayTag DamageTypeTag = Pair.Key;
        const FGameplayTag ResistanceTag = Pair.Value;

        checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs does not contain Tag: [%s] in ExecCals_Damage"), *ResistanceTag.ToString());
        
        const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];
        
        float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);
        if (DamageTypeValue <= 0.0f)
        {
            continue; //0 damage, no point looping through all types of damage
        }

        float ResistanceValue = 0.0f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, ResistanceValue);
        ResistanceValue = FMath::Clamp(ResistanceValue, 0.0f, 100.0f);

        DamageTypeValue *= (100.0f - ResistanceValue) / 100.0f;


        // SOLUTION USED IN COURSE for RADIAL DAMAGE
        // 1. Override TakeDamage in AuraCharacterBase
        // 2. Create delegate OnDamageDelegate, broadcast damage received in TakeDamage
        // 3. Bind lambda to OnDamageDelegate on the Victim here
        // 4. Call UGameplayStatics::ApplyRadialDamageWithFalloff to cause damage (this will result in 
        //    TakeDamage being called on the victim which will then broadcast OnDamageDelegate)
        // 5. In Lambda, set DamageTypeValue to the damage received from the broadcast

        if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
        {
            DamageTypeValue = UAuraAbilitySystemLibrary::GetRadialDamageWithFalloff(
                    TargetAvatar,
                    DamageTypeValue,
                    0.f,
                    UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
                    UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
                    UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
                    1.f);
        }

        Damage += DamageTypeValue;
    }

    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Damage after resistances: %f"), Damage));


    // Capture BlockChance on Target, and determine if there was a successful Block
    // If Block, halve the damage
    float TargetBlockChance = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(0.0f, TargetBlockChance);
    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
    Damage = bBlocked ? Damage *= 0.5f : Damage;


    // Armor (defender) and ArmorPenetration(Attacker)
    // ArmorPenetration ignores a percentage of the Target's Armor
    float TargetArmor = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
    TargetArmor = FMath::Max<float>(TargetArmor, 0.0f);

    float SourceArmorPenetration = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.0f);

    const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
    const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceLevel);

    const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
    const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetLevel);

    // We might want to scale the SourceArmorPenetration value here for example based on attacker and defender difference in levels
    const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.0f;
    Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.0f;


    // Critical Hits
    // Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
    // Double damage plus a bonus if critical hit
    float SourceCriticalHitChance = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
    SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.0f);

    float SourceCriticalHitDamage = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
    SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.0f);

    float TargetCriticalHitResistance = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
    TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.0f);

    const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
    const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetLevel);

    const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
    const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
    Damage = bCriticalHit ? Damage * 2.0f + SourceCriticalHitDamage : Damage;


    // Setting custom stuff in GameplayEffectContext
    UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
    UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);




    const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDefs) const
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
    for (TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuffs)
    {
        const FGameplayTag& DamageType = Pair.Key;
        const FGameplayTag& DebuffType = Pair.Value;
        const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.0f);
        if (TypeDamage > -1.0f)
        {
            // Determine if there was a successful debuff
            const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.0f);

            float TargetDebuffResistance = 0.0f;
            const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
            ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToDefs[ResistanceTag], EvaluationParameters, TargetDebuffResistance);
            TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.0f);

            const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.0f;
            const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
            if (bDebuff)
            {
                FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
                UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
                UAuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

                const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.0f);
                const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.0f);
                const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.0f);
                UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
                UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
                UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
            }
        }
    }
}