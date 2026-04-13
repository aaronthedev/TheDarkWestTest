#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TopDownPlayerState.generated.h"

class UTDWControllerInventory;
class UTDWAbilitySystem;

UCLASS(Blueprintable, BlueprintType)
class ATopDownPlayerState : public APlayerState
{
	GENERATED_BODY()	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTDWAbilitySystem* TDWAbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTDWControllerInventory* TDWInventoryComponent;
};
