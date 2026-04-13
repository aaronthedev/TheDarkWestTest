#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/Abilities/TDWAbility.h"
#include "TDWAbility_Attack.generated.h"

class UTDWGameplayEffect;

UCLASS(Abstract, BlueprintType, Blueprintable)
class THEDARKWESTTEST_API UTDWAbility_Attack : public UTDWAbility
{
	GENERATED_BODY()
public:
	UTDWAbility_Attack();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UTDWGameplayEffect> DamageEffectClass;

	/** Flat damage before any scaling. */
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "0"))
	float BaseDamage = 10.f;

	/** Returns the damage to apply. Override in subclasses for scaling. */
	virtual float GetDamageAmount() const { return BaseDamage; }

	void ApplyDamageToTarget(AActor* Target, const FGameplayAbilitySpecHandle& Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo) const;

	TArray<AActor*> GetActorsInRadius(const FVector& Origin, float Radius) const;
};
