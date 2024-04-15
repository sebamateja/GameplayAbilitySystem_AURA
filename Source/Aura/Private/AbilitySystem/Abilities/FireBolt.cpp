#include "AbilitySystem/Abilities/FireBolt.h"

#include "Kismet/KismetSystemLibrary.h" 

void UFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
    const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
    if (!bIsServer) return;

    const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
        GetAvatarActorFromActorInfo(), SocketTag);
    
    FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
    if (bOverridePitch) Rotation.Pitch = PitchOverride;

    const FVector Forward = Rotation.Vector();
    const FVector LeftOfSpread = Forward.RotateAngleAxis(-ProjectileSpread / 2.0f, FVector::UpVector);
    const FVector RightOfSpread = Forward.RotateAngleAxis(ProjectileSpread / 2.0f, FVector::UpVector);

    NumberOfProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
    
    if (NumberOfProjectiles > 1)
    {
        const float DeltaSpread = ProjectileSpread / (NumberOfProjectiles - 1);
        for (int32 i = 0; i < NumberOfProjectiles; i++)
        {
            const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
            UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation, SocketLocation + Direction * 100.0f, 4.0f, FLinearColor::Green, 3.0f);
        }
    }
    else
    {
        // Single projectile
    }
}

FString UFireBolt::GetDescription(int32 Level)
{
    const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);
    if (Level == 1)
    {
        return FString::Printf(TEXT(
            // Title
            "<Title>FIRE BOLT</>\n\n"

            // Details
            "<Small>Level: </><Level>%d</>\n"
            "<Small>ManaCost: </><ManaCost>%.1f</>\n"
            "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

            // Description
            "<Default>Launches a bolt of fire, exploding on impact and dealing: </>"
            "<Damage>%d</><Default> fire damage with a chance to burn</>"),

            Level, ManaCost, Cooldown, ScaledDamage);
    }
    else
    {
        return FString::Printf(TEXT(
            // Title
            "<Title>FIRE BOLT</>\n\n"

            // Details
            "<Small>Level: </><Level>%d</>\n"
            "<Small>ManaCost: </><ManaCost>%.1f</>\n"
            "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

            // Description
            "<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
            "<Damage>%d</><Default> fire damage with a chance to burn</>"),

            Level, ManaCost, Cooldown, FMath::Min(Level, NumberOfProjectiles), ScaledDamage);
    }
}

FString UFireBolt::GetNextLevelDescription(int32 Level)
{
    const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);
    return FString::Printf(TEXT(
        // Title
        "<Title>NEXT LEVEL</>\n\n"

        // Details
        "<Small>Level: </><Level>%d</>\n"
        "<Small>ManaCost: </><ManaCost>%.1f</>\n"
        "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

        // Description
        "<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
        "<Damage>%d</><Default> fire damage with a chance to burn</>"),

        Level, ManaCost, Cooldown, FMath::Min(Level, NumberOfProjectiles), ScaledDamage);
}