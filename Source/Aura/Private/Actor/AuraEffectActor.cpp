// Copyright Mark Shurgot


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraEffectActor::OnOverlap(AActor* OverlappedActor)
{
	if (OverlappedActor->ActorHasTag(FName("Enemy")) && !bApplyEffectToEnemies)
	{
		return;
	}
	for (const FGameplayEffectInfo& Info : Effects)
	{
		if (Info.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(OverlappedActor, Info.GameplayEffectClass);
		}
	}

	// if we have no infinite effects, check if we need to destroy this actor
	if (ActiveEffectHandles.Num() == 0 && bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* OverlappedActor)
{
	if (OverlappedActor->ActorHasTag(FName("Enemy")) && !bApplyEffectToEnemies)
	{
		return;
	}
	for (const FGameplayEffectInfo& Info : Effects)
	{
		if (Info.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(OverlappedActor, Info.GameplayEffectClass);
		}

		// Handling effect removal based on RemovalPolicy
		if (Info.RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			UAbilitySystemComponent* TargetAbilitySystemComponent =
				UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappedActor);
			if (!IsValid(TargetAbilitySystemComponent)) { return; }

			TArray<FActiveGameplayEffectHandle> HandlesToRemove;
			for (auto HandlePair : ActiveEffectHandles)
			{
				if (HandlePair.Value != TargetAbilitySystemComponent) { continue; }
				TargetAbilitySystemComponent->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
			// remove handles from map
			for (auto& Handle : HandlesToRemove)
			{
				ActiveEffectHandles.FindAndRemoveChecked(Handle);
			}
		}
	}
	// check if we need to destroy this actor
	if (bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectToEnemies)
	{
		return;
	}
	UAbilitySystemComponent* TargetAbilitySystemComponent =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetAbilitySystemComponent == nullptr)
	{
		return;
	}

	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = TargetAbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, ContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	// check duration policy on effect to see if it's infinite
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite)
	{
		for (const FGameplayEffectInfo& Info : Effects)
		{
			if (Info.GameplayEffectClass == GameplayEffectClass && Info.RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
			{
				// add to infinite effects map
				ActiveEffectHandles.Add(ActiveEffectHandle, TargetAbilitySystemComponent);
				break;
			}
		}
	}
}

