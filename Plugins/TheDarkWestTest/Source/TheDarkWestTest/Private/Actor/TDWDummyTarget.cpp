#include "Actor/TDWDummyTarget.h"
#include "GameplayAbility/Attributes/TDWAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

ATDWDummyTarget::ATDWDummyTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UTDWCommonAttributeSet>(TEXT("AttributeSet"));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
}

UAbilitySystemComponent* ATDWDummyTarget::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ATDWDummyTarget::BeginPlay()
{
	Super::BeginPlay();

	RandomPhase = FMath::FRandRange(0.f, PI * 2.f);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->SetNumericAttributeBase(UTDWCommonAttributeSet::GetMaxHealthAttribute(), MaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(UTDWCommonAttributeSet::GetHealthAttribute(), MaxHealth);
	CurrentHealth = MaxHealth;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UTDWCommonAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ATDWDummyTarget::OnHealthChanged);
}

void ATDWDummyTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawHealthLabel(DeltaTime);
}

void ATDWDummyTarget::DrawHealthLabel(float DeltaTime)
{
	BobTime += DeltaTime * BobSpeed;
	RandomColorTime += DeltaTime;

	const float Bob = FMath::Sin(BobTime + RandomPhase) * BobAmplitude;
	const FVector LabelPos = GetActorLocation() + FVector(0.f, 0.f, LabelHeightOffset + Bob);

	const float HPPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f);

	FColor LabelColor;
	if (bIsDead)
	{
		const float Flash = FMath::Abs(FMath::Sin(RandomColorTime * 4.f));
		LabelColor = FColor(uint8(255 * Flash), 0, 0);
	}
	else
	{
		LabelColor = FColor(uint8(255 * (1.f - HPPercent)), uint8(255 * HPPercent), 0);
	}

	const FString Label = bIsDead
		? FString::Printf(TEXT("DEAD  Respawn in %.1fs"), RespawnDelay)
		: FString::Printf(TEXT("%s\nHP %.0f / %.0f"), *GetName(), CurrentHealth, MaxHealth);

	DrawDebugString(GetWorld(), LabelPos, Label, nullptr, LabelColor, 0.f, true, 1.2f);
}

void ATDWDummyTarget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (bIsDead) return;

	CurrentHealth = FMath::Max(0.f, Data.NewValue);

	if (CurrentHealth <= 0.f)
	{
		bIsDead = true;

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ATDWDummyTarget::ResetDummy, RespawnDelay, false);
	}
}

void ATDWDummyTarget::ResetDummy()
{
	AbilitySystemComponent->SetNumericAttributeBase(UTDWCommonAttributeSet::GetHealthAttribute(), MaxHealth);
	CurrentHealth = MaxHealth;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	bIsDead = false;
}
