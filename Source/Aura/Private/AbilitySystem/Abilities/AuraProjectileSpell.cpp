// Copyright Mark Shurgot


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile()
{
	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
		if (CombatInterface)
		{
			FTransform SpawnTransform;
			const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
			SpawnTransform.SetLocation(SocketLocation);
			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass,
															SpawnTransform,
															GetOwningActorFromActorInfo(),
															Cast<APawn>(GetOwningActorFromActorInfo()),
															ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			Projectile->FinishSpawning(SpawnTransform);
		}
	}
}

