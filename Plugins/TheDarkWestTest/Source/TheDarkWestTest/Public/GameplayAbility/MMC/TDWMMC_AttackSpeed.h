#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "TDWMMC_AttackSpeed.generated.h"

UCLASS()
class THEDARKWESTTEST_API UTDWMMC_AttackSpeed : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UTDWMMC_AttackSpeed();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition AttackSpeedDef;
};

