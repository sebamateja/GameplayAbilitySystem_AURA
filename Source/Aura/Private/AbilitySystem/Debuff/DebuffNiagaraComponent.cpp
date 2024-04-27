#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/CombatInterface.h"

#include "AbilitySystemComponent.h" 

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
    bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
    Super::BeginPlay();

    // ASC might not be initialized at this point in time
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
    if (ASC)
    {
        ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
    }
    else if (CombatInterface)
    {
        CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
        {
            InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
        });
    }

    if (CombatInterface)
    {
        CombatInterface->GetOnDeathDelegateRef().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
    }
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    const bool bOwnerValid = IsValid(GetOwner());
    const bool bOwnerAlive = GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(GetOwner());
    if (NewCount > 0 && bOwnerValid && bOwnerAlive)
    {
        Activate();
    }
    else
    {
        Deactivate();
    }
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
    Deactivate();
}