#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

struct AuraDamageStatics
{
    // Target
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

    // Source
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);

    AuraDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
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

    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    // Gather tags from source and target
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // These are the parameters that we have to pass in to a specific
    // function in order to capture the attribute we are interested in (in our case Vigor)
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;





    // Get Damage Set by Caller Magnitude
    float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);


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

    // We might want to scale the SourceArmorPenetration value here for example based on attacker and defender difference in levels
    const float EffectiveArmor = TargetArmor *= (100 - SourceArmorPenetration * 0.25f) / 100.0f;
    Damage *= (100 - EffectiveArmor) / 100.0f;




    const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
}