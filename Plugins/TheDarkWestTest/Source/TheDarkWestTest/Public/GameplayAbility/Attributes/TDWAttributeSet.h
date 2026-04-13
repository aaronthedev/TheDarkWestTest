#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "TDWAttributeSet.generated.h"

UCLASS(BlueprintType)
class THEDARKWESTTEST_API UTDWCommonAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Mana;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxMana;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxStamina;
	
	
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTDWCommonAttributeSet, Health)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Health)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Health)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health)
	
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTDWCommonAttributeSet, MaxHealth)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxHealth)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxHealth)
	
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTDWCommonAttributeSet, Mana)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Mana)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Mana)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Mana)
	
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTDWCommonAttributeSet, MaxMana)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxMana)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxMana)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxMana)
	
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTDWCommonAttributeSet, Stamina)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Stamina)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Stamina)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Stamina)
	
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTDWCommonAttributeSet, MaxStamina)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxStamina)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxStamina)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxStamina)
	
	
};

UCLASS()
class THEDARKWESTTEST_API UTDWCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData AttackSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData AttackDamage;
	
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTDWCombatAttributeSet, AttackSpeed)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(AttackSpeed)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(AttackSpeed)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(AttackSpeed)
	
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTDWCombatAttributeSet, AttackDamage)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(AttackDamage)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(AttackDamage)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(AttackDamage)
	
};

