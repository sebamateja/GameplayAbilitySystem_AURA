#include "AbilitySystem/Abilities/BeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
    if (HitResult.bBlockingHit)
    {
        MouseHitLocation = HitResult.ImpactPoint;
        MouseHitActor = HitResult.GetActor();
    }
    else
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
    }
}

void UBeamSpell::StoreOwnerVariables()
{
    if (CurrentActorInfo)
    {
        OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
        OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
    }
}

void UBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
    check(OwnerCharacter);
    if (OwnerCharacter->Implements<UCombatInterface>())
    {
        USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter);
        if (Weapon)
        {
            const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));

            TArray<AActor*> ActorsToIgnore;
            ActorsToIgnore.Add(OwnerCharacter);
            FHitResult HitResult;
            UKismetSystemLibrary::SphereTraceSingle(
                OwnerCharacter, 
                SocketLocation, 
                BeamTargetLocation, 
                10.0f, 
                TraceTypeQuery1, 
                false, 
                ActorsToIgnore, 
                EDrawDebugTrace::None, 
                HitResult,
                true);

            if (HitResult.bBlockingHit)
            {
                MouseHitLocation = HitResult.ImpactPoint;
                MouseHitActor = HitResult.GetActor();
            }
        }
    }
}

void UBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
    ActorsToIgnore.Add(MouseHitActor);

    TArray<AActor*> OverlappingActors;
    UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
        GetAvatarActorFromActorInfo(), 
        OverlappingActors, 
        ActorsToIgnore,
        850.0f,
        MouseHitActor->GetActorLocation());

    // int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
    int32 NumAdditionalTargets = 5;

    UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());
}