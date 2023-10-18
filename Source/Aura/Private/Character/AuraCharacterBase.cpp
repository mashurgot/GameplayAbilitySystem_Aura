// Copyright Mark Shurgot


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAuraCharacterBase::GetAttributeSet() const
{
	return AttributeSet;
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
	//
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes);
	ApplyEffectToSelf(DefaultSecondaryAttributes);
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> Effect, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(Effect);
    
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(Effect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

