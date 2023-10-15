// Copyright Mark Shurgot

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	void OnRep_PlayerState();
	virtual void PossessedBy(AController* NewController) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

private:
	void InitAbilityActorInfo();	
};
