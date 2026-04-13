#include "GameplayAbility/Effects/TDWGameplayEffect_Cooldown.h"

UTDWGameplayEffect_Cooldown::UTDWGameplayEffect_Cooldown()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Cooldown"));

	DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
}


