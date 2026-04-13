#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TDWAbilityInputBinding.generated.h"

class UInputAction;

/**
 * Pairs an Enhanced Input action with a gameplay ability class.
 * Add entries to ATopDown55Character::DefaultAbilities in the Blueprint
 * defaults — the character will grant and bind them automatically.
 */
USTRUCT(BlueprintType)
struct THEDARKWESTTEST_API FTDWAbilityInputBinding
{
	GENERATED_BODY()

	/** The Enhanced Input action that triggers the ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction;

	/** The ability class to grant and activate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	/** The default key assigned to this ability. Can be changed at runtime via RebindAbilityKey(). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FKey DefaultKey;
};


