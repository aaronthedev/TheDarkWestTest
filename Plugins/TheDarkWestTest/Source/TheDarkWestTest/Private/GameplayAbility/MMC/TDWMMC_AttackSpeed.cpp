#include "GameplayAbility/MMC/TDWMMC_AttackSpeed.h"
#include "GameplayAbility/Attributes/TDWAttributeSet.h"

UTDWMMC_AttackSpeed::UTDWMMC_AttackSpeed()
{
	AttackSpeedDef.AttributeToCapture = UTDWCombatAttributeSet::GetAttackSpeedAttribute();
	AttackSpeedDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	AttackSpeedDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(AttackSpeedDef);
}

float UTDWMMC_AttackSpeed::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float AttackSpeed = 1.f;
	GetCapturedAttributeMagnitude(AttackSpeedDef, Spec, EvaluationParameters, AttackSpeed);

	return FMath::Max(AttackSpeed, 0.1f);
}

