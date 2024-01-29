#include "AbilitySystem/Data/AttributeInfo.h"
#include "Aura/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
    for (const FAuraAttributeInfo& Info : AttributeInformation)
    {
        if (Info.AttributeTag.MatchesTagExact(AttributeTag)) // == can also be used
        {
            return Info;
        }
    }

    if (bLogNotFound)
    {
        UE_LOG(LogAura, Error, TEXT("Cant find Info for AttributeTag [%s] on AttributeInfo [%s]"), *AttributeTag.ToString(), *GetNameSafe(this));
    }
    return FAuraAttributeInfo();
}