#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, *AIOwner->GetName());
    GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Green, *ActorOwner->GetName());

    APawn* OwningPawn = AIOwner->GetPawn();

    const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

    float ClosestDistance = TNumericLimits<float>::Max();
    AActor* ClosestActor = nullptr;
    for (AActor* Actor : ActorsWithTag)
    {
        if (IsValid(Actor) && IsValid(OwningPawn))
        {
            const float Distance = OwningPawn->GetDistanceTo(Actor);
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestActor = Actor;
            }
        }
        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Orange, *Actor->GetName());
    }

    UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
    UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
}