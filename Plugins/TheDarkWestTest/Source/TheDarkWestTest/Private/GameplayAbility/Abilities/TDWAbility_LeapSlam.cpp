#include "GameplayAbility/Abilities/TDWAbility_LeapSlam.h"
#include "GameplayAbility/Attributes/TDWAttributeSet.h"
#include "GameplayAbility/TargetActor/TDWTargetActor_CursorTrace.h"
#include "GameplayAbility/Effects/TDWGameplayEffect.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_WaitMovementModeChange.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveFloat.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

UTDWAbility_LeapSlam::UTDWAbility_LeapSlam()
{
}

void UTDWAbility_LeapSlam::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedAttackSpeed = GetAttackSpeedFromASC(ActorInfo);

	ATDWTargetActor_CursorTrace* TargetActor = GetWorld()->SpawnActor<ATDWTargetActor_CursorTrace>();

	UAbilityTask_WaitTargetData* TargetTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActor);

	TargetTask->ValidData.AddDynamic(this, &UTDWAbility_LeapSlam::OnTargetDataReceived);
	TargetTask->Cancelled.AddDynamic(this, &UTDWAbility_LeapSlam::OnTargetDataCancelled);
	BindTargetingInput();
	TargetTask->ReadyForActivation();
}

void UTDWAbility_LeapSlam::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	ACharacter* Character = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	FVector TargetLocation = Character->GetActorLocation();
	if (Data.Num() > 0 && Data.Data[0].IsValid())
	{
		const FHitResult* HitResult = Data.Data[0]->GetHitResult();
		if (HitResult && HitResult->bBlockingHit)
		{
			TargetLocation = HitResult->Location;
		}
	}

	const FVector CharLoc = Character->GetActorLocation();

	CachedLandingLocation = TargetLocation;

	const float Gravity        = FMath::Abs(GetWorld()->GetGravityZ());
	const float HorizontalDist = FVector::Dist2D(CharLoc, TargetLocation);
	const FVector Direction    = (TargetLocation - CharLoc).GetSafeNormal2D();

	const float ZVelocity      = (CachedAttackSpeed * Gravity) / 2.f;
	const float HorizontalSpeed = HorizontalDist / CachedAttackSpeed;

	Character->LaunchCharacter(Direction * HorizontalSpeed + FVector(0.f, 0.f, ZVelocity), true, true);

	if (SlamMontage)
	{
		const float PlayRate = SlamMontage->GetPlayLength() / CachedAttackSpeed;

		UAbilityTask_PlayMontageAndWait* MontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SlamMontage, PlayRate);
		MontageTask->OnCompleted.AddDynamic(this, &UTDWAbility_LeapSlam::OnMontageCompleted);
		MontageTask->OnBlendOut.AddDynamic(this, &UTDWAbility_LeapSlam::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UTDWAbility_LeapSlam::OnMontageCancelled);
		MontageTask->OnCancelled.AddDynamic(this, &UTDWAbility_LeapSlam::OnMontageCancelled);
		MontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* EventTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ImpactEventTag, nullptr, false);
		EventTask->EventReceived.AddDynamic(this, &UTDWAbility_LeapSlam::OnImpactEventReceived);
		EventTask->ReadyForActivation();
	}
	else
	{
		UAbilityTask_WaitMovementModeChange* LandTask =
			UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(this, MOVE_Walking);
		LandTask->OnChange.AddDynamic(this, &UTDWAbility_LeapSlam::OnLanded);
		LandTask->ReadyForActivation();
	}
}

void UTDWAbility_LeapSlam::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UTDWAbility_LeapSlam::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (!DebugLevelUpAction && !DebugLevelDownAction) return;

	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
	if (!PC) return;

	if (DebugMappingContext)
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (auto* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				Sub->AddMappingContext(DebugMappingContext, 0);
			}
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		if (DebugLevelUpAction)
		{
			DebugLevelUpHandle = EIC->BindAction(
				DebugLevelUpAction, ETriggerEvent::Started,
				this, &UTDWAbility_LeapSlam::Debug_GemLevelUp).GetHandle();
		}
		if (DebugLevelDownAction)
		{
			DebugLevelDownHandle = EIC->BindAction(
				DebugLevelDownAction, ETriggerEvent::Started,
				this, &UTDWAbility_LeapSlam::Debug_GemLevelDown).GetHandle();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("LeapSlam: PlayerController has no EnhancedInputComponent — debug level keys not bound."));
	}
}

void UTDWAbility_LeapSlam::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
	if (PC)
	{
		if (DebugMappingContext)
		{
			if (ULocalPlayer* LP = PC->GetLocalPlayer())
			{
				if (auto* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
				{
					Sub->RemoveMappingContext(DebugMappingContext);
				}
			}
		}

		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			EIC->RemoveBindingByHandle(DebugLevelUpHandle);
			EIC->RemoveBindingByHandle(DebugLevelDownHandle);
		}
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UTDWAbility_LeapSlam::Debug_GemLevelUp()
{
	const int32 Before = GetGemLevel();
	if (Before >= MaxGemLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LeapSlam] Already at max gem level (%d)."), MaxGemLevel);
		return;
	}
	AccumulatedXP = 0;
	DoLevelUp();
	UE_LOG(LogTemp, Warning, TEXT("[LeapSlam] DEBUG level up: %d -> %d"), Before, GetGemLevel());
}

void UTDWAbility_LeapSlam::Debug_GemLevelDown()
{
	const int32 Before = GetGemLevel();
	if (Before <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LeapSlam] Already at minimum gem level (1)."));
		return;
	}

	if (const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo())
	{
		if (UAbilitySystemComponent* ASC = Info->AbilitySystemComponent.Get())
		{
			if (FGameplayAbilitySpec* S = ASC->FindAbilitySpecFromClass(GetClass()))
			{
				S->Level = FMath::Max(1, S->Level - 1);
				ASC->MarkAbilitySpecDirty(*S);
				AccumulatedXP = 0;
				UE_LOG(LogTemp, Warning, TEXT("[LeapSlam] DEBUG level down: %d -> %d"), Before, GetGemLevel());
			}
		}
	}
}

int32 UTDWAbility_LeapSlam::GetGemLevel() const
{
	if (const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo())
	{
		if (UAbilitySystemComponent* ASC = Info->AbilitySystemComponent.Get())
		{
			if (const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(GetClass()))
			{
				return FMath::Max(1, Spec->Level);
			}
		}
	}
	return 1;
}

float UTDWAbility_LeapSlam::GetDefaultXPThreshold(int32 Level) const
{
	return 100.f * FMath::Pow(static_cast<float>(Level), 2.5f);
}

float UTDWAbility_LeapSlam::GetXPToNextLevel() const
{
	const int32 Level = GetGemLevel();
	if (Level >= MaxGemLevel) return 0.f;

	return XPThresholdCurve
		? XPThresholdCurve->GetFloatValue(static_cast<float>(Level))
		: GetDefaultXPThreshold(Level);
}

float UTDWAbility_LeapSlam::GetXPProgressPercent() const
{
	if (GetGemLevel() >= MaxGemLevel) return 1.f;
	const float Required = GetXPToNextLevel();
	return Required > 0.f ? FMath::Clamp(static_cast<float>(AccumulatedXP) / Required, 0.f, 1.f) : 1.f;
}

void UTDWAbility_LeapSlam::DoLevelUp()
{
	if (const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo())
	{
		if (UAbilitySystemComponent* ASC = Info->AbilitySystemComponent.Get())
		{
			if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(GetClass()))
			{
				const int32 OldLevel = Spec->Level;
				Spec->Level++;
				ASC->MarkAbilitySpecDirty(*Spec);
				OnGemLevelUp.Broadcast(OldLevel, Spec->Level);
			}
		}
	}
}

void UTDWAbility_LeapSlam::AddGemXP(int32 Amount)
{
	if (Amount <= 0 || GetGemLevel() >= MaxGemLevel) return;

	AccumulatedXP += Amount;

	while (GetGemLevel() < MaxGemLevel)
	{
		const float Required = GetXPToNextLevel();
		if (static_cast<float>(AccumulatedXP) < Required) break;

		AccumulatedXP -= FMath::RoundToInt(Required);
		DoLevelUp();
	}

	if (GetGemLevel() >= MaxGemLevel)
	{
		AccumulatedXP = 0;
	}
}

float UTDWAbility_LeapSlam::GetScaledRadius() const
{
	const float Scale = RadiusScaleCurve
		? RadiusScaleCurve->GetFloatValue(static_cast<float>(GetGemLevel()))
		: 1.f;
	return SlamRadius * Scale;
}

float UTDWAbility_LeapSlam::GetScaledKnockback() const
{
	const float Scale = KnockbackScaleCurve
		? KnockbackScaleCurve->GetFloatValue(static_cast<float>(GetGemLevel()))
		: 1.f;
	return KnockbackStrength * Scale;
}

float UTDWAbility_LeapSlam::GetDamageAmount() const
{
	const float Scale = DamageScaleCurve
		? DamageScaleCurve->GetFloatValue(static_cast<float>(GetGemLevel()))
		: 1.f;
	return BaseDamage * Scale;
}

float UTDWAbility_LeapSlam::GetAttackSpeedFromASC(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		bool bFound = false;
		const float Value = ASC->GetGameplayAttributeValue(
			UTDWCombatAttributeSet::GetAttackSpeedAttribute(), bFound);
		if (bFound)
		{
			return FMath::Max(Value, 0.1f);
		}
	}
	return 1.f;
}

void UTDWAbility_LeapSlam::OnImpactEventReceived(FGameplayEventData Payload)
{
	ACharacter* Character = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	const FVector ImpactLocation = CachedLandingLocation.IsNearlyZero() && Character
		? Character->GetActorLocation()
		: CachedLandingLocation;

	ApplyLandingImpact(ImpactLocation);
}

void UTDWAbility_LeapSlam::OnMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UTDWAbility_LeapSlam::OnMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UTDWAbility_LeapSlam::ApplyLandingImpact(const FVector& ImpactLocation)
{
	if (ImpactVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, ImpactLocation);
	}

	const float ScaledRadius    = GetScaledRadius();
	const float ScaledKnockback = GetScaledKnockback();

	TArray<AActor*> Targets = GetActorsInRadius(ImpactLocation, ScaledRadius);

	const FGameplayAbilitySpecHandle SpecHandle    = GetCurrentAbilitySpecHandle();
	const FGameplayAbilityActorInfo* ActorInfo     = GetCurrentActorInfo();
	const FGameplayAbilityActivationInfo ActivInfo = GetCurrentActivationInfo();

	for (AActor* Target : Targets)
	{
		if (!Target) continue;

		ApplyDamageToTarget(Target, SpecHandle, ActorInfo, ActivInfo);

		if (SecondaryEffectClass)
		{
			if (UAbilitySystemComponent* TargetASC =
				UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
			{
				FGameplayEffectSpecHandle SecSpec =
					MakeOutgoingGameplayEffectSpec(SpecHandle, ActorInfo, ActivInfo, SecondaryEffectClass);
				if (SecSpec.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*SecSpec.Data.Get());
				}
			}
		}

		if (ScaledKnockback > 0.f)
		{
			if (ACharacter* TargetChar = Cast<ACharacter>(Target))
			{
				const FVector KnockDir = (Target->GetActorLocation() - ImpactLocation).GetSafeNormal2D();
				TargetChar->LaunchCharacter(KnockDir * ScaledKnockback, true, true);
			}
		}
	}
}

void UTDWAbility_LeapSlam::OnLanded(EMovementMode PrevMovementMode)
{
	ACharacter* Character = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	const FVector ImpactLocation = Character ? Character->GetActorLocation() : CachedLandingLocation;

	ApplyLandingImpact(ImpactLocation);
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

