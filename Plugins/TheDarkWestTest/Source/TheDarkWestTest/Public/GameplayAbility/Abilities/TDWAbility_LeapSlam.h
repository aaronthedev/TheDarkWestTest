#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/Abilities/TDWAbility_Attack.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "TDWAbility_LeapSlam.generated.h"

class UAnimMontage;
class UParticleSystem;
class UCurveFloat;
class UInputMappingContext;
class UInputAction;

/** Fired whenever the gem gains a level. OldLevel is 1-based, NewLevel = OldLevel + 1. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLeapSlamGemLevelUp, int32, OldLevel, int32, NewLevel);

UCLASS(BlueprintType, Blueprintable)
class THEDARKWESTTEST_API UTDWAbility_LeapSlam : public UTDWAbility_Attack
{
	GENERATED_BODY()
public:
	UTDWAbility_LeapSlam();

	// -----------------------------------------------------------------------
	// Gem Progression API
	// -----------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Gem Progression")
	void AddGemXP(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Gem Progression")
	int32 GetGemLevel() const;

	UFUNCTION(BlueprintPure, Category = "Gem Progression")
	int32 GetGemXP() const { return AccumulatedXP; }

	UFUNCTION(BlueprintPure, Category = "Gem Progression")
	float GetXPToNextLevel() const;

	UFUNCTION(BlueprintPure, Category = "Gem Progression")
	float GetXPProgressPercent() const;

	UPROPERTY(BlueprintAssignable, Category = "Gem Progression")
	FOnLeapSlamGemLevelUp OnGemLevelUp;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// -----------------------------------------------------------------------
	// Leap Slam core
	// -----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Leap Slam")
	TObjectPtr<UAnimMontage> SlamMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Leap Slam")
	FGameplayTag ImpactEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Leap Slam|Impact", meta = (ClampMin = "0"))
	float SlamRadius = 300.f;


	UPROPERTY(EditDefaultsOnly, Category = "Leap Slam|Arc",
		meta = (ClampMin = "0.05", ClampMax = "2.0"))
	float ArcHeightRatio = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "Leap Slam|Impact")
	TObjectPtr<UParticleSystem> ImpactVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Leap Slam|Impact", meta = (ClampMin = "0"))
	float KnockbackStrength = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Leap Slam|Impact")
	TSubclassOf<UTDWGameplayEffect> SecondaryEffectClass;

	// -----------------------------------------------------------------------
	// Gem Progression settings
	// -----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Gem Progression", meta = (ClampMin = "1"))
	int32 MaxGemLevel = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Gem Progression")
	TObjectPtr<UCurveFloat> XPThresholdCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Gem Progression|Scaling")
	TObjectPtr<UCurveFloat> RadiusScaleCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Gem Progression|Scaling")
	TObjectPtr<UCurveFloat> KnockbackScaleCurve;

	/** Multiplies BaseDamage per gem level. If unset, damage stays flat. */
	UPROPERTY(EditDefaultsOnly, Category = "Gem Progression|Scaling")
	TObjectPtr<UCurveFloat> DamageScaleCurve;

	virtual float GetDamageAmount() const override;

	// -----------------------------------------------------------------------
	// Debug / Testing — assign these in the Blueprint defaults to enable
	// -----------------------------------------------------------------------

	/**
	 * Input Mapping Context that contains the two debug actions below.
	 * It is added automatically when the ability is granted and removed when the ability is revoked.
	 * Leave empty to disable debug input.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Gem Progression|Debug")
	TObjectPtr<UInputMappingContext> DebugMappingContext;

	/** Input action that increments gem level by 1 (press once = +1 level). */
	UPROPERTY(EditDefaultsOnly, Category = "Gem Progression|Debug")
	TObjectPtr<UInputAction> DebugLevelUpAction;

	/** Input action that decrements gem level by 1 (press once = -1 level). */
	UPROPERTY(EditDefaultsOnly, Category = "Gem Progression|Debug")
	TObjectPtr<UInputAction> DebugLevelDownAction;

private:
	float   CachedAttackSpeed       = 1.f;
	FVector CachedLandingLocation   = FVector::ZeroVector;

	UPROPERTY()
	int32 AccumulatedXP = 0;

	/** Handles for the persistent debug input bindings so we can remove them cleanly. */
	uint32 DebugLevelUpHandle   = 0;
	uint32 DebugLevelDownHandle = 0;

	float GetAttackSpeedFromASC(const FGameplayAbilityActorInfo* ActorInfo) const;
	void  ApplyLandingImpact(const FVector& ImpactLocation);
	float GetDefaultXPThreshold(int32 Level) const;
	float GetScaledRadius() const;
	float GetScaledKnockback() const;
	void  DoLevelUp();

	/** Debug helpers — called by the bound input actions. */
	void Debug_GemLevelUp();
	void Debug_GemLevelDown();

	UFUNCTION() void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data);
	UFUNCTION() void OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data);
	UFUNCTION() void OnImpactEventReceived(FGameplayEventData Payload);
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageCancelled();
	UFUNCTION() void OnLanded(EMovementMode PrevMovementMode);
};
