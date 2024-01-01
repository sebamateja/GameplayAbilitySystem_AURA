#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
    // We want to spawn projectile on server. In this case the server will be in charge of moving it,
    // handling its location. And the clients will just see a replicated version of the projectile.
    // So we need to set bReplicates=true in AuraProjectile.cpp
    
    const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //HasAuthority(&ActivationInfo);
    if (!bIsServer) return;

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
    if (CombatInterface)
    {
        const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
        
        FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
        Rotation.Pitch = 0.0f;

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
            const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
            // GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("FireBolt Damage: %f"), ScaledDamage));
            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaledDamage);
            
            Projectile->DamageEffectSpecHandle = SpecHandle;
        }

        Projectile->FinishSpawning(SpawnTransform);
    }
}