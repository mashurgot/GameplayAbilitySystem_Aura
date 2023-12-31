// Copyright Mark Shurgot


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();

	TagsToAttributes.Add(AuraGameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Vital_Mana, GetManaAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Fire, GetResistance_FireAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Lightening, GetResistance_LighteningAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Arcane, GetResistance_ArcaneAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Physical, GetResistance_PhysicalAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate attributes to everyone
	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	// Replicate secondary attributes to everyone
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	// Replicate resistances to everyone
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Lightening, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Arcane, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Physical, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp Health between 0 and MaxHealth
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealth.GetCurrentValue());
	}
	
	// Clamp Mana between 0 and MaxMana
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxMana.GetCurrentValue());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// source = causer of the effect, target = target of the effect (owner of the attribute set)
	// Create the FEffectProperties struct
	FEffectProperties EffectProperties;

	// Populate the EffectProperties struct with data from the Data parameter
	SetEffectProperties(Data, EffectProperties);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Ensure Health does not exceed MaxHealth and is above 0
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		// TODO placeholder to show damage from damage effects
		UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s, Health: %f"), *Data.Target.GetAvatarActor()->GetName(), GetHealth());
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Ensure Mana does not exceed MaxMana and is above 0
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		
		if (LocalIncomingDamage > 0)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(EffectProperties.Target.AvatarActor))
				{
					CombatInterface->Die();
				}
			}
			else
			{
				FGameplayTagContainer HitReactTagContainer;
				HitReactTagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
				EffectProperties.Target.AbilitySystemComponent->TryActivateAbilitiesByTag(HitReactTagContainer);
			}

			const bool bBlock = UAuraAbilitySystemLibrary::IsBlockedHit(EffectProperties.EffectContextHandle);
			const bool bCrit = UAuraAbilitySystemLibrary::IsCriticalHit(EffectProperties.EffectContextHandle);
			ShowFloatingText(EffectProperties, LocalIncomingDamage, bBlock, bCrit);
		}
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& EffectProperties, const float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	// show damage text
	if (EffectProperties.Source.Character != EffectProperties.Target.Character)
	{
		if (EffectProperties.Source.Character)
		{
			if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(EffectProperties.Source.Character->Controller))
			{
				PC->ShowDamageNumber(Damage, EffectProperties.Target.Character, bBlockedHit, bCriticalHit);
				return;
			}
		}
		if (EffectProperties.Target.Character)
		{
			if(AAuraPlayerController* PC = Cast<AAuraPlayerController>(EffectProperties.Target.Character->Controller))
			{
				PC->ShowDamageNumber(Damage, EffectProperties.Target.Character, bBlockedHit, bCriticalHit);
			}
		}
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties) const
{
	// Set the EffectContextHandle
	EffectProperties.EffectContextHandle = Data.EffectSpec.GetContext();
	
	// Setting up the Source attributes
	UAbilitySystemComponent* SourceASC = EffectProperties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		EffectProperties.Source.AbilitySystemComponent = SourceASC;
		EffectProperties.Source.AvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
		EffectProperties.Source.PlayerController = SourceASC->AbilityActorInfo->PlayerController.Get();

		// If there's no player controller, try to get it from the AvatarActor
		if (EffectProperties.Source.PlayerController == nullptr && EffectProperties.Source.AvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(EffectProperties.Source.AvatarActor))
			{
				EffectProperties.Source.PlayerController = Pawn->GetController<APlayerController>();
			}
		}
		
		// Try to get the character from the Controller's pawn
		if (EffectProperties.Source.PlayerController)
		{
			EffectProperties.Source.Character = Cast<ACharacter>(EffectProperties.Source.PlayerController->GetPawn());
		}
	}
	
	// Setting up the Target attributes
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		EffectProperties.Target.AbilitySystemComponent = Data.Target.AbilityActorInfo->AbilitySystemComponent.Get();
		EffectProperties.Target.AvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		EffectProperties.Target.PlayerController = Data.Target.AbilityActorInfo->PlayerController.Get();
		EffectProperties.Target.Character = Cast<ACharacter>(EffectProperties.Target.AvatarActor);

		// If there's no player controller, try to get it from the AvatarActor
		if (EffectProperties.Target.PlayerController == nullptr && EffectProperties.Target.AvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(EffectProperties.Target.AvatarActor))
			{
				EffectProperties.Target.PlayerController = Pawn->GetController<APlayerController>();
			}
		}
	}
}


void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_Resistance_Fire(const FGameplayAttributeData& OldResistance_Fire) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resistance_Fire, OldResistance_Fire);
}

void UAuraAttributeSet::OnRep_Resistance_Lightening(const FGameplayAttributeData& OldResistance_Lightening) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resistance_Lightening, OldResistance_Lightening);
}

void UAuraAttributeSet::OnRep_Resistance_Arcane(const FGameplayAttributeData& OldResistance_Arcane) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resistance_Arcane, OldResistance_Arcane);
}

void UAuraAttributeSet::OnRep_Resistance_Physical(const FGameplayAttributeData& OldResistance_Physical) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resistance_Physical, OldResistance_Physical);
}
