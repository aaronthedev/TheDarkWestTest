#include "GameplayAbility/Notify/TDWAnimNotify_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"

void UTDWAnimNotify_GameplayEvent::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	HitActors.Reset();
}

void UTDWAnimNotify_GameplayEvent::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!Owner) return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	if (!ASC) return;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	Owner->GetWorld()->OverlapMultiByChannel(Overlaps, Owner->GetActorLocation(), FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(TraceRadius), Params);
	
	DrawDebugSphere(Owner->GetWorld(), Owner->GetActorLocation(), TraceRadius, 8, FColor::Red, false, 0.f);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor) continue;
		if (HitActors.Contains(HitActor)) continue;

		HitActors.Add(HitActor);

		FGameplayEventData Payload;
		Payload.Instigator = Owner;
		Payload.Target = HitActor;
		ASC->HandleGameplayEvent(EventTag, &Payload);
	}
}

void UTDWAnimNotify_GameplayEvent::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	HitActors.Reset();
}

