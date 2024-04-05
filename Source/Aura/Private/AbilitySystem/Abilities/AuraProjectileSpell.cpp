#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h" 

#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
    // We want to spawn projectile on server. In this case the server will be in charge of moving it,
    // handling its location. And the clients will just see a replicated version of the projectile.
    // So we need to set bReplicates=true in AuraProjectile.cpp
    
    const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //HasAuthority(&ActivationInfo);
    if (!bIsServer) return;

    const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
        GetAvatarActorFromActorInfo(), SocketTag);
    
    FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
    if (bOverridePitch)
    {
        Rotation.Pitch = PitchOverride;
    }

    FTransform SpawnTransform;
    SpawnTransform.SetLocation(SocketLocation);
    SpawnTransform.SetRotation(Rotation.Quaternion());
    

    AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
        ProjectileClass, 
        SpawnTransform, 
        GetOwningActorFromActorInfo(), 
        Cast<APawn>(GetOwningActorFromActorInfo()),
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    if (DamageEffectClass)
    {
        const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

        FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
        EffectContextHandle.SetAbility(this);
        EffectContextHandle.AddSourceObject(Projectile);
        FHitResult HitResult;
        HitResult.Location = ProjectileTargetLocation;
        EffectContextHandle.AddHitResult(HitResult);

        const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
        
        const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

        for (auto& Pair : DamageTypes)
        {
            const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
            // GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("FireBolt Damage: %f"), ScaledDamage));
            // UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaledDamage);
            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
        }

        
        Projectile->DamageEffectSpecHandle = SpecHandle;
    }

    Projectile->FinishSpawning(SpawnTransform);
}

FString UAuraProjectileSpell::GetDescription(int32 Level)
{
    const int Damage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
    if (Level == 1)
    {
        return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Default>Launches a bolt of fire, exploding on impact and dealing: </> <Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), Damage, Level);
    }
    else
    {
        return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Default>Launches %d bolts of fire, exploding on impact and dealing: </> <Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, NumberOfProjectiles), Damage, Level);
    }
}

FString UAuraProjectileSpell::GetNextLevelDescription(int32 Level)
{
    const int Damage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
    return FString::Printf(TEXT("<Title>NEXT LEVEL</>\n\n<Default>Launches %d bolts of fire, exploding on impact and dealing: </> <Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, NumberOfProjectiles), Damage, Level);
}