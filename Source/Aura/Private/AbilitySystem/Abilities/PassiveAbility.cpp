#include "AbilitySystem/Abilities/PassiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
    {
        // Binding to delegate that sends info which passive ability needs to be deactivated
        AuraASC->DeactivatePassiveAbility.AddUObject(this, &UPassiveAbility::ReceiveDeactivation);
    }
}

void UPassiveAbility::ReceiveDeactivation(const FGameplayTag& AbilityTag)
{
    if(AbilityTags.HasTagExact(AbilityTag))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}