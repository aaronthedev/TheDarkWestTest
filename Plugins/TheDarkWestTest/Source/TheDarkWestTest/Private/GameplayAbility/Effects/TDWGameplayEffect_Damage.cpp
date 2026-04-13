#include "GameplayAbility/Effects/TDWGameplayEffect_Damage.h"
#include "GameplayAbility/Attributes/TDWAttributeSet.h"

UTDWGameplayEffect_Damage::UTDWGameplayEffect_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));

	FGameplayModifierInfo DamageModifier;
	DamageModifier.Attribute    = UTDWCommonAttributeSet::GetHealthAttribute();
	DamageModifier.ModifierOp   = EGameplayModOp::Additive;
	DamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(DamageModifier);
}


