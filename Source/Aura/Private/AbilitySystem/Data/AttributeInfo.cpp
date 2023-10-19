// Copyright Mark Shurgot


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag AttributeTag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag == AttributeTag)
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		// log error
		UE_LOG(LogTemp, Error, TEXT("AttributeInfo for %s not found"), *AttributeTag.ToString());
	}
	return FAuraAttributeInfo();
}
