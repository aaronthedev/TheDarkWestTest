#include "TopDown55Character.h"
#include "GameplayAbility/Component/TDWAbilityComponent.h"
#include "GameplayAbility/Attributes/TDWAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

ATopDown55Character::ATopDown55Character()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	TDWAbilitySystem  = CreateDefaultSubobject<UTDWAbilitySystem>(TEXT("TDWAbilitySystem"));
	CommonAttributes  = CreateDefaultSubobject<UTDWCommonAttributeSet>(TEXT("CommonAttributes"));
	CombatAttributes  = CreateDefaultSubobject<UTDWCombatAttributeSet>(TEXT("CombatAttributes"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATopDown55Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (TDWAbilitySystem)
	{
		TDWAbilitySystem->InitAbilityActorInfo(this, this);

		TDWAbilitySystem->SetNumericAttributeBase(UTDWCommonAttributeSet::GetMaxHealthAttribute(), 100.f);
		TDWAbilitySystem->SetNumericAttributeBase(UTDWCommonAttributeSet::GetHealthAttribute(),    100.f);
		TDWAbilitySystem->SetNumericAttributeBase(UTDWCommonAttributeSet::GetMaxManaAttribute(),   100.f);
		TDWAbilitySystem->SetNumericAttributeBase(UTDWCommonAttributeSet::GetManaAttribute(),      100.f);
		TDWAbilitySystem->SetNumericAttributeBase(UTDWCommonAttributeSet::GetMaxStaminaAttribute(), 100.f);
		TDWAbilitySystem->SetNumericAttributeBase(UTDWCommonAttributeSet::GetStaminaAttribute(),    100.f);

		TDWAbilitySystem->SetNumericAttributeBase(UTDWCombatAttributeSet::GetAttackDamageAttribute(), 10.f);
		TDWAbilitySystem->SetNumericAttributeBase(UTDWCombatAttributeSet::GetAttackSpeedAttribute(),   1.f);
	}
}

UAbilitySystemComponent* ATopDown55Character::GetAbilitySystemComponent() const
{
	return TDWAbilitySystem;
}

void ATopDown55Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATopDown55Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	APlayerController* PC = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = PC
		? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer())
		: nullptr;

	if (TDWAbilitySystem)
	{
		TDWAbilitySystem->InitializeInputBinding(EIC, Subsystem);
	}
}

