// Copyright Mark Shurgot

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Interaction/CombatInterface.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
protected:
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:

	UPROPERTY(ReplicatedUsing = OnRep_Level, VisibleAnywhere)
	int32 Level	= 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
};
