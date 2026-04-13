#include "GameplayAbility/Tasks/TDWAbilityTask_WaitTargetDataReusable.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"

UTDWAbilityTask_WaitTargetDataReusable* UTDWAbilityTask_WaitTargetDataReusable::WaitTargetDataReusable(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
	AGameplayAbilityTargetActor* InTargetActor)
{
	UTDWAbilityTask_WaitTargetDataReusable* Task =
		NewAbilityTask<UTDWAbilityTask_WaitTargetDataReusable>(OwningAbility, TaskInstanceName);
	Task->TargetClass      = nullptr;
	Task->TargetActor      = InTargetActor;
	Task->ConfirmationType = ConfirmationType;
	return Task;
}

void UTDWAbilityTask_WaitTargetDataReusable::OnDestroy(bool AbilityEnded)
{
	if (TargetActor)
	{
		if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
		{
			// Remove accumulated confirm/cancel delegate bindings so they don't fire multiple times next activation
			ASC->GenericLocalConfirmCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::ConfirmTargeting);
			ASC->GenericLocalCancelCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::CancelTargeting);

			// Remove from SpawnedTargetActors so it isn't pushed again on top of a stale entry
			ASC->SpawnedTargetActors.Remove(TargetActor);
		}

		// Null before Super so the base class does not call Destroy() on the cached actor
		TargetActor = nullptr;
	}

	Super::OnDestroy(AbilityEnded);
}


