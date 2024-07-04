#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "FireBall.generated.h"

UCLASS()
class AURA_API AFireBall : public AAuraProjectile
{
	GENERATED_BODY()
public:

protected:
	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
