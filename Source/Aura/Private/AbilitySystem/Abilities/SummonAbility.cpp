#include "AbilitySystem/Abilities/SummonAbility.h"

TArray<FVector> USummonAbility::GetSpawnLocations()
{
    const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
    const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
    const float DeltaSpread = SpawnSpread / NumberOfMinions;

    const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.0f, FVector::UpVector);
    TArray<FVector> SpawnLocations;

    for (int32 i = 0; i < NumberOfMinions; i++)
    {
        const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
        FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);

        FHitResult Hit;
        GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.0f, 0.0f, 400.0f), ChosenSpawnLocation - FVector(0.0f, 0.0f, 400.0f), ECC_Visibility);
        if (Hit.bBlockingHit)
        {
            ChosenSpawnLocation = Hit.ImpactPoint;
        }

        SpawnLocations.Add(ChosenSpawnLocation);

        // DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 20.0f, 12, FColor::Blue, false, 3.0f);
        // UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.0f, FLinearColor::Green, 3.0f);
        // DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.0f, 12, FColor::Red, false, 3.0f);
        // DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.0f, 12, FColor::Red, false, 3.0f);
    }

    return SpawnLocations;
}

TSubclassOf<APawn> USummonAbility::GetRandomMinionClass()
{
    const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
    return MinionClasses[Selection];
}