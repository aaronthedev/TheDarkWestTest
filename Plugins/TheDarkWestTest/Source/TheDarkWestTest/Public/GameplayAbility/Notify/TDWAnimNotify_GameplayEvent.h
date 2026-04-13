#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "TDWAnimNotify_GameplayEvent.generated.h"

UCLASS(DisplayName = "Gameplay Event (Trace Window)")
class THEDARKWESTTEST_API UTDWAnimNotify_GameplayEvent : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "GameplayEvent")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, Category = "GameplayEvent")
	float TraceRadius = 150.f;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;
};

