#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
    FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.0f);
    float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor, bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
    FDamageEffectParams Params;
    Params.WorldContextObject = GetAvatarActorFromActorInfo();
    Params.DamageGameplayEffectClass = DamageEffectClass;
    Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
    Params.AbilityLevel = GetAbilityLevel();
    Params.DamageType = DamageType;
    Params.DebuffChance = DebuffChance;
    Params.DebuffDamage = DebuffDamage;
    Params.DebuffFrequency = DebuffFrequency;
    Params.DebuffDuration = DebuffDuration;
    Params.DeathImpulseMagnitude = DeathImpulseMagnitude;

    if (IsValid(TargetActor))
    {
        FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
        if (bOverridePitch)
        {
            Rotation.Pitch = PitchOverride;
        }
        const FVector ToTarget = Rotation.Vector();
        if (!bOverrideDeathImpulse)
        {
            Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
        }
    }

    if (bOverrideDeathImpulse)
    {
        DeathImpulseDirectionOverride.Normalize();
        Params.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude;
        if (bOverridePitch)
        {
            FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
            DeathImpulseRotation.Pitch = PitchOverride;
            Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
        }
    }

    if (bIsRadialDamage)
    {
        Params.bIsRadialDamage = bIsRadialDamage;
        Params.RadialDamageOrigin = RadialDamageOrigin;
        Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
        Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
    }

    return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
    return Damage.GetValueAtLevel(GetAbilityLevel());
}