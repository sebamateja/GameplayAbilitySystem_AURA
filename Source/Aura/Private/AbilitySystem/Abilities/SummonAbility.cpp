#include "AbilitySystem/Abilities/SummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

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
        const FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);
        SpawnLocations.Add(ChosenSpawnLocation);
        DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 20.0f, 12, FColor::Blue, false, 3.0f);

        UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.0f, FLinearColor::Green, 3.0f);
        DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.0f, 12, FColor::Red, false, 3.0f);
        DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.0f, 12, FColor::Red, false, 3.0f);
    }

    // UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + RightOfSpread * MaxSpawnDistance, 4.0f, FLinearColor::Green, 3.0f);



    // const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.0f, FVector::UpVector);
    // UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + LeftOfSpread * MaxSpawnDistance, 4.0f, FLinearColor::Gray, 3.0f);

    // DrawDebugSphere(GetWorld(), Location + LeftOfSpread * MinSpawnDistance, 15.0f, 12, FColor::Red, false, 3.0f);
    // DrawDebugSphere(GetWorld(), Location + LeftOfSpread * MaxSpawnDistance, 15.0f, 12, FColor::Red, false, 3.0f);

    return SpawnLocations;
}