#include "GameplayAbility/Abilities/TDWAbility.h"
#include "GameplayAbility/Attributes/TDWAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

// Binds ConfirmTargetInputAction / CancelTargetInputAction to LocalInputConfirm / LocalInputCancel
void UTDWAbility::BindTargetingInput()
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo) return;

	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
	if (!Pawn || !PC) return;

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(Pawn->InputComponent);
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!EIC || !ASC || !Subsystem) return;

	// Build a temporary mapping context that maps the confirm/cancel keys
	TargetingMappingContext = NewObject<UInputMappingContext>(this);

	if (ConfirmTargetInputAction)
	{
		if (ConfirmTargetKey.IsValid())
			TargetingMappingContext->MapKey(ConfirmTargetInputAction, ConfirmTargetKey);

		ConfirmBindingHandle = EIC->BindAction(ConfirmTargetInputAction, ETriggerEvent::Started,
			ASC, &UAbilitySystemComponent::LocalInputConfirm).GetHandle();
	}

	if (CancelTargetInputAction)
	{
		if (CancelTargetKey.IsValid())
			TargetingMappingContext->MapKey(CancelTargetInputAction, CancelTargetKey);

		CancelBindingHandle = EIC->BindAction(CancelTargetInputAction, ETriggerEvent::Started,
			ASC, &UAbilitySystemComponent::LocalInputCancel).GetHandle();
	}

	// Priority 10 — high enough to sit above ability and movement contexts
	Subsystem->AddMappingContext(TargetingMappingContext, 10);
}

void UTDWAbility::UnbindTargetingInput(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (ConfirmBindingHandle == 0 && CancelBindingHandle == 0 && !TargetingMappingContext) return;
	if (!ActorInfo) return;

	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
	if (!Pawn || !PC) return;

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(Pawn->InputComponent);
	if (EIC)
	{
		if (ConfirmBindingHandle != 0) { EIC->RemoveBindingByHandle(ConfirmBindingHandle); ConfirmBindingHandle = 0; }
		if (CancelBindingHandle  != 0) { EIC->RemoveBindingByHandle(CancelBindingHandle);  CancelBindingHandle  = 0; }
	}

	if (TargetingMappingContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(TargetingMappingContext);
		}
		TargetingMappingContext = nullptr;
	}
}

void UTDWAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UnbindTargetingInput(ActorInfo);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UTDWAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (ManaCost <= 0.f) return true;

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) return false;

	bool bFound = false;
	const float CurrentMana = ASC->GetGameplayAttributeValue(UTDWCommonAttributeSet::GetManaAttribute(), bFound);
	return bFound && CurrentMana >= ManaCost;
}

void UTDWAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (ManaCost <= 0.f || !GetCostGameplayEffect()) return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, GetCostGameplayEffect()->GetClass());
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("Data.ManaCost")), -ManaCost);

	(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

const FGameplayTagContainer* UTDWAbility::GetCooldownTags() const
{
	CachedCooldownTags.Reset();
	if (AbilityCooldownTag.IsValid())
		CachedCooldownTags.AddTag(AbilityCooldownTag);
	return &CachedCooldownTags;
}

void UTDWAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownDuration <= 0.f || !GetCooldownGameplayEffect()) return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
		Handle, ActorInfo, ActivationInfo, GetCooldownGameplayEffect()->GetClass());
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("Data.Cooldown")), CooldownDuration);

	if (AbilityCooldownTag.IsValid())
		SpecHandle.Data->DynamicGrantedTags.AddTag(AbilityCooldownTag);

	(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

