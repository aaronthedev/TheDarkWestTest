#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "TDWDummyTarget.generated.h"

class UAbilitySystemComponent;
class UTDWCommonAttributeSet;

UCLASS()
class THEDARKWESTTEST_API ATDWDummyTarget : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ATDWDummyTarget();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTDWCommonAttributeSet> AttributeSet;

	/** Starting and respawn health value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dummy")
	float MaxHealth = 100.f;

	/** Seconds before the dummy resets after death. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dummy")
	float RespawnDelay = 3.f;

	UPROPERTY(EditAnywhere, Category = "Dummy|Label")
	float LabelHeightOffset = 120.f;

	UPROPERTY(EditAnywhere, Category = "Dummy|Label")
	float BobAmplitude = 12.f;

	UPROPERTY(EditAnywhere, Category = "Dummy|Label")
	float BobSpeed = 2.f;

private:
	void OnHealthChanged(const struct FOnAttributeChangeData& Data);
	void ResetDummy();
	void DrawHealthLabel(float DeltaTime);

	FTimerHandle RespawnTimerHandle;
	bool bIsDead = false;

	float BobTime = 0.f;
	float RandomPhase = 0.f;
	float RandomColorTime = 0.f;
	float CurrentHealth = 0.f;
};
