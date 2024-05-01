#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
    bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
    Super::BeginPlay();

    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
    {
        AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
    }
    else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
    {
        CombatInterface->GetOnASCRegisteredDelegateRef().AddLambda([this](UAbilitySystemComponent* ASC){
            if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
            {
                AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
            }
        });
    }
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
    if (AbilityTag.MatchesTagExact(PassiveSpellTag))
    {
        if (bActivate && !IsActive())
        {
            Activate();
        }
        else
        {
            Deactivate();
        }
    }
}