// Copyright Mark Shurgot

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AuraAttributeSet.generated.h"

// create attribute accessors macro
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


USTRUCT(BlueprintType)
struct FEntityAttributes
{
	GENERATED_USTRUCT_BODY()

	// Pointer to the Ability System Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Attributes")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	// Pointer to the Avatar Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Attributes")
	AActor* AvatarActor = nullptr;

	// Pointer to the Controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Attributes")
	APlayerController* PlayerController = nullptr;

	// Pointer to the Character (assuming this is a Character Actor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Attributes")
	ACharacter* Character = nullptr;
};

USTRUCT(BlueprintType)
struct FEffectProperties
{
	GENERATED_USTRUCT_BODY()

	FEffectProperties() {}
	
	// Attributes for the source
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Properties")
	FEntityAttributes Source;

	// Attributes for the target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Properties")
	FEntityAttributes Target;

	// Effect Context Handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Attributes")
	FGameplayEffectContextHandle EffectContextHandle;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

private:

	// Helper function to get the effect properties
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties) const;
};
