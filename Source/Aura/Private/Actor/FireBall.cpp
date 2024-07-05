#include "Actor/FireBall.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void AFireBall::BeginPlay()
{
    Super::BeginPlay();
    InitialLocation = GetActorLocation();
    ApexLocation = InitialLocation + (GetActorForwardVector() * TravelDistance);
    StartOutgoingTimeline();
}

void AFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsValidOverlap(OtherActor)) return;

    if (IgnoreList.Contains(OtherActor)) return;

    if (HasAuthority())
	{
		// We only need to apply GameplayEffect on server because the result of this GE will be replicated because of attributes
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;

			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
            IgnoreList.Add(OtherActor);
		}
	}
}

void AFireBall::EmptyIgnoreList()
{
    IgnoreList.Empty();
}