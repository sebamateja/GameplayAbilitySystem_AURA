#include "AbilitySystem/MMC/MMCMaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMCMaxMana::UMMCMaxMana()
{
    IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
    IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
    IntelligenceDef.bSnapshot = false; // snapshoting is about timing

    RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMCMaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    // Gather tags from source and target
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // These are the parameters that we have to pass in to a specific
    // function in order to capture the attribute we are interested in (in our case Vigor)
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    float Intelligence = 0.0f;
    GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluationParameters, Intelligence);
    Intelligence = FMath::Max<float>(Intelligence, 0.0f);

   ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
   const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

    return 50.0f + 2.5f * Intelligence + 15.0f * PlayerLevel;
}