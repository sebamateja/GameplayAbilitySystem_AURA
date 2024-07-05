#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "FireBall.generated.h"

UCLASS()
class AURA_API AFireBall : public AAuraProjectile
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;

	UFUNCTION(BlueprintCallable)
    void EmptyIgnoreList();
     
    UPROPERTY(BlueprintReadWrite)
    TArray<AActor*> IgnoreList;

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;

protected:
	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(BlueprintReadOnly)
	FVector InitialLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector ApexLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
	float TravelDistance = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
	float ExplodeDistance = 220.0f;
};
