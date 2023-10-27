// Copyright Mark Shurgot

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

struct FActiveGameplayEffectHandle;
class UGameplayEffect;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

USTRUCT(BlueprintType)
struct FGameplayEffectInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect Info")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect Info")
	EEffectApplicationPolicy ApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect Info")
	EEffectRemovalPolicy RemovalPolicy = EEffectRemovalPolicy::DoNotRemove;
};


UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();


protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	bool bApplyEffectToEnemies = false;
	
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* OverlappedActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* OverlappedActor);
	
	UFUNCTION(BlueprintCallable, Category = "Applied Effects")
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	// Replace single-instance properties with an array of FGameplayEffectInfo
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	TArray<FGameplayEffectInfo> Effects;
	
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	float ActorLevel = 1.0f;
private:

};
