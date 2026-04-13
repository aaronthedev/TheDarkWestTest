#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "TDWAbilityTask_WaitTargetDataReusable.generated.h"

/**
 * Variant of UAbilityTask_WaitTargetData that does NOT destroy the target actor when the task ends.
 */
UCLASS()
class THEDARKWESTTEST_API UTDWAbilityTask_WaitTargetDataReusable : public UAbilityTask_WaitTargetData
{
	GENERATED_BODY()

public:
	static UTDWAbilityTask_WaitTargetDataReusable* WaitTargetDataReusable(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
		AGameplayAbilityTargetActor* InTargetActor);

protected:
	virtual void OnDestroy(bool AbilityEnded) override;
};



