#include "GameplayAbility/TargetActor/TDWTargetActor_CursorTrace.h"

#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameFramework/PlayerController.h"

ATDWTargetActor_CursorTrace::ATDWTargetActor_CursorTrace()
{
	// Client performs the cursor trace; UAbilityTask_WaitTargetData replicates
	// the resulting FGameplayAbilityTargetDataHandle to the server.
	ShouldProduceTargetDataOnServer = false;
}

void ATDWTargetActor_CursorTrace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
}

void ATDWTargetActor_CursorTrace::ConfirmTargetingAndContinue()
{
	FGameplayAbilityTargetDataHandle DataHandle;

	if (OwningAbility)
	{
		APlayerController* PC = Cast<APlayerController>(
			OwningAbility->GetCurrentActorInfo()->PlayerController.Get());

		if (PC)
		{
			FHitResult HitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

			// Always package the result (caller checks bBlockingHit)
			FGameplayAbilityTargetData_SingleTargetHit* Data =
				new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			DataHandle.Add(Data);
		}
	}

	TargetDataReadyDelegate.Broadcast(DataHandle);
}

