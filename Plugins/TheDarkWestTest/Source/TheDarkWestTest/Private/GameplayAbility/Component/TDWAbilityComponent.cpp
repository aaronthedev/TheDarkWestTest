#include "GameplayAbility/Component/TDWAbilityComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

void UTDWAbilitySystem::InitializeInputBinding(UEnhancedInputComponent* EIC, UEnhancedInputLocalPlayerSubsystem* Subsystem)
{
	if (!EIC || !Subsystem) return;

	CachedEIC       = EIC;
	CachedSubsystem = Subsystem;

	AbilityMappingContext = NewObject<UInputMappingContext>(this, TEXT("AbilityMappingContext"));

	for (const FPendingAbility& Pending : PendingAbilities)
	{
		if (Pending.InputAction && Pending.DefaultKey.IsValid())
			AbilityMappingContext->MapKey(Pending.InputAction, Pending.DefaultKey);

		AbilityEntries.Add(Pending.AbilityClass, { Pending.InputAction, Pending.SpecHandle });
		if (Pending.InputAction)
		{
			ActionToAbilityMap.Add(Pending.InputAction, Pending.AbilityClass);
			EIC->BindAction(Pending.InputAction, ETriggerEvent::Started, this,
				&UTDWAbilitySystem::OnAbilityInputPressed);
		}
	}
	PendingAbilities.Empty();

	Subsystem->AddMappingContext(AbilityMappingContext, 1);
}

void UTDWAbilitySystem::GrantAndBindAbility(TSubclassOf<UGameplayAbility> AbilityClass,
	UInputAction* InputAction, FKey DefaultKey)
{
	if (!AbilityClass || AbilityEntries.Contains(AbilityClass)) return;

	FGameplayAbilitySpecHandle SpecHandle = GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));

	if (AbilityMappingContext && CachedSubsystem.IsValid() && CachedEIC.IsValid())
	{
		RegisterKeyInContext(InputAction, DefaultKey);
		AbilityEntries.Add(AbilityClass, { InputAction, SpecHandle });
		if (InputAction)
		{
			ActionToAbilityMap.Add(InputAction, AbilityClass);
			CachedEIC->BindAction(InputAction, ETriggerEvent::Started, this,
				&UTDWAbilitySystem::OnAbilityInputPressed);
		}
	}
	else
	{
		PendingAbilities.Add({ AbilityClass, InputAction, DefaultKey, SpecHandle });
	}
}

void UTDWAbilitySystem::RemoveAndUnbindAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	FAbilityEntry* Entry = AbilityEntries.Find(AbilityClass);
	if (!Entry) return;

	ClearAbility(Entry->SpecHandle);

	if (AbilityMappingContext && Entry->InputAction)
	{
		if (CachedSubsystem.IsValid())
			CachedSubsystem->RemoveMappingContext(AbilityMappingContext);

		AbilityMappingContext->UnmapAction(Entry->InputAction);

		if (CachedSubsystem.IsValid())
			CachedSubsystem->AddMappingContext(AbilityMappingContext, 1);
	}

	ActionToAbilityMap.Remove(Entry->InputAction);
	AbilityEntries.Remove(AbilityClass);
}

void UTDWAbilitySystem::RebindAbilityKey(TSubclassOf<UGameplayAbility> AbilityClass, FKey NewKey)
{
	if (!NewKey.IsValid() || !AbilityMappingContext || !CachedSubsystem.IsValid()) return;

	FAbilityEntry* Entry = AbilityEntries.Find(AbilityClass);
	if (!Entry || !Entry->InputAction) return;

	CachedSubsystem->RemoveMappingContext(AbilityMappingContext);
	AbilityMappingContext->UnmapAction(Entry->InputAction);
	AbilityMappingContext->MapKey(Entry->InputAction, NewKey);
	CachedSubsystem->AddMappingContext(AbilityMappingContext, 1);
}

void UTDWAbilitySystem::RegisterKeyInContext(UInputAction* InputAction, FKey Key)
{
	if (!InputAction || !Key.IsValid() || !AbilityMappingContext) return;

	if (CachedSubsystem.IsValid())
		CachedSubsystem->RemoveMappingContext(AbilityMappingContext);

	AbilityMappingContext->MapKey(InputAction, Key);

	if (CachedSubsystem.IsValid())
		CachedSubsystem->AddMappingContext(AbilityMappingContext, 1);
}

void UTDWAbilitySystem::OnAbilityInputPressed(const FInputActionInstance& ActionInstance)
{
	if (const TSubclassOf<UGameplayAbility>* AbilityClass = ActionToAbilityMap.Find(ActionInstance.GetSourceAction()))
		TryActivateAbilityByClass(*AbilityClass);
}


