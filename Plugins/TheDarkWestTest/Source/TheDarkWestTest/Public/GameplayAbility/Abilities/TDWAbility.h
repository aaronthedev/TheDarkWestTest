#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "TDWAbility.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS(BlueprintType)
class THEDARKWESTTEST_API UTDWAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<UInputAction> ConfirmTargetInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	FKey ConfirmTargetKey = EKeys::LeftMouseButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<UInputAction> CancelTargetInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	FKey CancelTargetKey = EKeys::RightMouseButton;

	/** Mana deducted on each activation. Requires CostGameplayEffectClass = TDWGameplayEffect_ManaCost. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Costs", meta = (ClampMin = "0"))
	float ManaCost = 0.f;

	/** Cooldown duration in seconds. Requires CooldownGameplayEffectClass = TDWGameplayEffect_Cooldown. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown", meta = (ClampMin = "0"))
	float CooldownDuration = 0.f;

	/** Tag granted to the ASC while this ability is on cooldown (e.g. Cooldown.Ability.LeapSlam). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	FGameplayTag AbilityCooldownTag;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	void BindTargetingInput();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	uint32 ConfirmBindingHandle = 0;
	uint32 CancelBindingHandle  = 0;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> TargetingMappingContext;

	mutable FGameplayTagContainer CachedCooldownTags;

	void UnbindTargetingInput(const FGameplayAbilityActorInfo* ActorInfo);
};

