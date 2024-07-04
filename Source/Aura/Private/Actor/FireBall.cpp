#include "Actor/FireBall.h"

void AFireBall::BeginPlay()
{
    Super::BeginPlay();
    InitialLocation = GetActorLocation();
    ApexLocation = InitialLocation + (GetActorForwardVector() * TravelDistance);
    StartOutgoingTimeline();
}

void AFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}