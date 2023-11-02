#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    
    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

    // How often the server will try to update clients
    // So as changes occur on the server for the player state, then the server
    // will be sending updates out to all clients so that they cann sync up with the server version
    // Default value for Player State to receive update is about 500
    // Other games like Lyra or Fortnite are using more frequent updates (about 100)
    NetUpdateFrequency = 100.0;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}