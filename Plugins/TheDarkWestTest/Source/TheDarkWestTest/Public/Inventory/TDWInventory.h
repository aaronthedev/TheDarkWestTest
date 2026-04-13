#pragma once

#include "CoreMinimal.h"
#include "Itemization.h"
#include "Components/ActorComponent.h"
#include "TDWInventory.generated.h"

class ATDWPickUpActor;
class UTDWItemDefinition;


UCLASS(BlueprintType)
class THEDARKWESTTEST_API UTDWControllerInventory : public UActorComponent
{
	GENERATED_BODY()
public:
	
	UTDWControllerInventory();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UTDWItemDefinition>> StartingItems;
	
	UFUNCTION(BlueprintCallable)
	void AddItemFromClass(TSubclassOf<UTDWItemDefinition> ItemClass);
	
	UFUNCTION(BlueprintCallable)
	void Pickup(ATDWPickUpActor* PickUpActor);
	
	UFUNCTION(BlueprintCallable)
	void AddItem(UTDWItemDefinition* Item);
	
	UFUNCTION(BlueprintCallable)
	void DropItem(int ItemIndex);
	
	UFUNCTION(BlueprintCallable)
	const TArray<FTDWItemInstance>& GetItems() const { return Items; }
	
	virtual bool EvaluateAttachment(int ItemIndex, int AttachmentSlot, UTDWItemDefinition* Attachment) const;
	
protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<FTDWItemInstance> Items;
	
	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> ItemMeshes;
		
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerPC;
	
};
