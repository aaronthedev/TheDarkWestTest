#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TDWTargetActor_CursorTrace.generated.h"

/**
 * Performs an instant cursor hit-trace on the owning client and broadcasts
 * the result as FGameplayAbilityTargetDataHandle.
 * ShouldProduceTargetDataOnServer = false ensures the trace always runs on
 * the client; UAbilityTask_WaitTargetData replicates the handle to the server.
 */
UCLASS()
class THEDARKWESTTEST_API ATDWTargetActor_CursorTrace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATDWTargetActor_CursorTrace();

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
};

