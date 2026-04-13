#include "GameplayAbility/Effects/TDWGameplayEffect_ManaCost.h"
#include "GameplayAbility/Attributes/TDWAttributeSet.h"

UTDWGameplayEffect_ManaCost::UTDWGameplayEffect_ManaCost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.ManaCost"));

	FGameplayModifierInfo ManaCostModifier;
	ManaCostModifier.Attribute    = UTDWCommonAttributeSet::GetManaAttribute();
	ManaCostModifier.ModifierOp   = EGameplayModOp::Additive;
	ManaCostModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(ManaCostModifier);
}


