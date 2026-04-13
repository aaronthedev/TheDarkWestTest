#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "Itemization.generated.h"

UENUM(BlueprintType)
enum class ETDWItemType : uint8
{
	None,
	SkillGem,
	SupportGem,
	UncutGem,
	Weapon,
	Armor,
	Currency
};

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class THEDARKWESTTEST_API UTDWItemFragment : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual void LoadAssets() const {};
	
};


USTRUCT(BlueprintType)
struct FTDWAttachDefinition
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ETDWItemType Type;
};

UCLASS(BlueprintType, Blueprintable)
class THEDARKWESTTEST_API UTDWItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETDWItemType ItemType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTDWAttachDefinition> AttachableDefinitions;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Item", GetFName());
	}
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Fragments")
	TArray<UTDWItemFragment*> Fragments;

	template<typename T>
	const T* FindFragment() const
	{
		for (const UTDWItemFragment* Frag : Fragments)
		{
			if (const T* Casted = Cast<T>(Frag))
				return Casted;
		}
		return nullptr;
	}
	
	void LoadAllFragments() const
	{
		for (const UTDWItemFragment* Frag : Fragments)
		{
			if (Frag)
				Frag->LoadAssets();
		}
	}
};

USTRUCT(BlueprintType)
struct FTDWItemAttachable 
{
	GENERATED_BODY()
public:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FPrimaryAssetId ItemId;
};

USTRUCT(BlueprintType)
struct FTDWItemInstance
{
	GENERATED_BODY()
	friend class UTDWControllerInventory;
public:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UTDWItemDefinition* ItemDefinition;  
	
	FTDWItemInstance() : ItemDefinition(nullptr) {}
	FTDWItemInstance(UTDWItemDefinition* InDef) : ItemDefinition(InDef) {}
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FTDWItemAttachable> Attachments;
};


USTRUCT(BlueprintType)
struct FTDWAbilityGrant
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FKey DefaultKey;
};

UCLASS(BlueprintType)
class THEDARKWESTTEST_API UTDWGrantAbilityFragment : public UTDWItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTDWAbilityGrant> AbilitiesToGrant;
};

UCLASS(BlueprintType)
class THEDARKWESTTEST_API UTDWMeshFragment : public UTDWItemFragment
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Socket;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale = 1.f;
	
	virtual void LoadAssets() const override
	{	
		Mesh.LoadSynchronous();
	}
};
