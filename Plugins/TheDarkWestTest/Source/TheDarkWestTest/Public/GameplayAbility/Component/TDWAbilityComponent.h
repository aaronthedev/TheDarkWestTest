#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TDWAbilityComponent.generated.h"

class UInputAction;
class UInputMappingContext;
class UEnhancedInputComponent;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionInstance;

UCLASS(BlueprintType)
class THEDARKWESTTEST_API UTDWAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void InitializeInputBinding(UEnhancedInputComponent* EIC, UEnhancedInputLocalPlayerSubsystem* Subsystem);
	void GrantAndBindAbility(TSubclassOf<UGameplayAbility> AbilityClass, UInputAction* InputAction, FKey DefaultKey);
	void RemoveAndUnbindAbility(TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void RebindAbilityKey(TSubclassOf<UGameplayAbility> AbilityClass, FKey NewKey);


private:
	struct FAbilityEntry
	{
		TObjectPtr<UInputAction>   InputAction;
		FGameplayAbilitySpecHandle SpecHandle;
	};

	struct FPendingAbility
	{
		TSubclassOf<UGameplayAbility> AbilityClass;
		TObjectPtr<UInputAction>      InputAction;
		FKey                          DefaultKey;
		FGameplayAbilitySpecHandle    SpecHandle;
	};

	TMap<TSubclassOf<UGameplayAbility>, FAbilityEntry>           AbilityEntries;
	TMap<TObjectPtr<UInputAction>, TSubclassOf<UGameplayAbility>> ActionToAbilityMap;
	TArray<FPendingAbility>                                       PendingAbilities;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> AbilityMappingContext;

	TWeakObjectPtr<UEnhancedInputComponent>            CachedEIC;
	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> CachedSubsystem;

	void RegisterKeyInContext(UInputAction* InputAction, FKey Key);
	void OnAbilityInputPressed(const FInputActionInstance& ActionInstance);
};
