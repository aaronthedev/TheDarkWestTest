#include "GameplayAbility/Abilities/TDWAbility_Attack.h"
#include "GameplayAbility/Effects/TDWGameplayEffect.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetSystemLibrary.h"

UTDWAbility_Attack::UTDWAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}


void UTDWAbility_Attack::ApplyDamageToTarget(AActor* Target, const FGameplayAbilitySpecHandle& Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo) const
{
	if (!DamageEffectClass || !Target) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, DamageEffectClass);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("Data.Damage")), -GetDamageAmount());

	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

TArray<AActor*> UTDWAbility_Attack::GetActorsInRadius(const FVector& Origin, float Radius) const
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
		UEngineTypes::ConvertToObjectType(ECC_Pawn)
	};

	TArray<AActor*> Ignored = { GetAvatarActorFromActorInfo() };
	TArray<FHitResult> Hits;

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(), Origin, Origin, Radius,
		ObjectTypes, false, Ignored,
		EDrawDebugTrace::ForDuration,
		Hits, true,
		FLinearColor::Red, FLinearColor::Green, 2.f);

	TArray<AActor*> Actors;
	for (const FHitResult& Hit : Hits)
	{
		if (AActor* Actor = Hit.GetActor())
			Actors.AddUnique(Actor);
	}
	return Actors;
}

