// Copyright Mark Shurgot


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitMana(100);
	InitMaxMana(100);
	InitHealth(50);
	InitMaxHealth(100);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate attributes to everyone
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
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

		// If there's no player controller, try to get it from the AvatarActor
		if (EffectProperties.Target.PlayerController == nullptr && EffectProperties.Target.AvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(EffectProperties.Target.AvatarActor))
			{
				EffectProperties.Target.PlayerController = Pawn->GetController<APlayerController>();
			}
		}

		// Try to get the character from the Controller's pawn
		if (EffectProperties.Target.PlayerController)
		{
			EffectProperties.Target.Character = Cast<ACharacter>(EffectProperties.Target.PlayerController->GetPawn());
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
