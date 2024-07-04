#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraAbilityTypes.h"
#include "AuraProjectile.generated.h"

class UAudioComponent;
class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FDamageEffectParams DamageEffectParams;

	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	void OnHit();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.0f;

	bool bHit = false;

	UPROPERTY(VisibleAnywhere, Category = "Custom")
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere, Category = "Custom")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Custom")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Custom")
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
