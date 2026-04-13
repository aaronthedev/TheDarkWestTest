#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Itemization.h"
#include "TDWPickUpActor.generated.h"

UCLASS(BlueprintType, Blueprintable)
class THEDARKWESTTEST_API ATDWPickUpActor : public AActor
{
	GENERATED_BODY()
public:
	
	ATDWPickUpActor();
	
	// Item received when picked up
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTDWItemDefinition* ItemDefinition;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;
	
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
};
