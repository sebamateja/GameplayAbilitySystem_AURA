#include "AbilitySystem/Abilities/FireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/FireBall.h"

TArray<AFireBall*> UFireBlast::SpawnFireBalls()
{
    TArray<AFireBall*> FireBalls;

    const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
    const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
    TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.0f, NumFireBalls);

    for (const FRotator& Rotator : Rotators)
    {
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(Location);
        SpawnTransform.SetRotation(Rotator.Quaternion());
        AFireBall* FireBall = GetWorld()->SpawnActorDeferred<AFireBall>(
            FireBallClass, 
            SpawnTransform, 
            GetOwningActorFromActorInfo(),
            CurrentActorInfo->PlayerController->GetPawn(),
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

        FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
        FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
        FireBall->SetOwner(GetAvatarActorFromActorInfo());

        FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();
        FireBall->ExplosionDamageParams.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());

        FireBalls.Add(FireBall);
        FireBall->FinishSpawning(SpawnTransform);
    }

    return FireBalls;
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