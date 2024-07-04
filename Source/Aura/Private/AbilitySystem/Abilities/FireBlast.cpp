#include "AbilitySystem/Abilities/FireBlast.h"

TArray<AFireBall*> UFireBlast::SpawnFireBalls()
{
    return TArray<AFireBall*>();
}

FString UFireBlast::GetDescription(int32 Level)
{
    const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);

    return FString::Printf(TEXT(
        // Title
        "<Title>FIRE BLAST</>\n\n"

        // Details
        "<Small>Level: </><Level>%d</>\n"
        "<Small>ManaCost: </><ManaCost>%.1f</>\n"
        "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

        // Number of Fire Balls
        "<Default>Launches %d </>"
        "<Default>fire balls in all directions, each coming back and </>"
        "<Default>exploding upon return, causing </>"

        "<Damage>%d</><Default> radial fire damage with a chance to burn.</>"),

        Level, ManaCost, Cooldown, NumFireBalls, ScaledDamage);
}

FString UFireBlast::GetNextLevelDescription(int32 Level)
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

        // Number of Fire Balls
        "<Default>Launches %d </>"
        "<Default>fire balls in all directions, each coming back and </>"
        "<Default>exploding upon return, causing </>"

        "<Damage>%d</><Default> radial fire damage with a chance to burn.</>"),

        Level, ManaCost, Cooldown, NumFireBalls, ScaledDamage);
}