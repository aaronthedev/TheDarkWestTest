#include "Inventory/TDWInventory.h"

#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayAbility/Component/TDWAbilityComponent.h"
#include "Inventory/TDWPickUpActor.h"

UTDWControllerInventory::UTDWControllerInventory()
{
}

void UTDWControllerInventory::AddItemFromClass(TSubclassOf<UTDWItemDefinition> ItemClass)
{
}

void UTDWControllerInventory::Pickup(ATDWPickUpActor* PickUpActor)
{
	if (!OwnerPC)
		return;
	if (!OwnerPC->HasAuthority())
		return;
	if (!PickUpActor || !PickUpActor->ItemDefinition)
		return;

	AddItem(PickUpActor->ItemDefinition);
	PickUpActor->Destroy();
}

void UTDWControllerInventory::AddItem(UTDWItemDefinition* Item)
{
	if (!Item) return;

	Items.Add(FTDWItemInstance{Item});

	ACharacter* OwnerChar = OwnerPC ? Cast<ACharacter>(OwnerPC->GetPawn()) : nullptr;
	UTDWAbilitySystem* ASC = OwnerChar
		? Cast<UTDWAbilitySystem>(Cast<IAbilitySystemInterface>(OwnerChar)->GetAbilitySystemComponent())
		: nullptr;

	if (ASC)
	{
		if (const UTDWGrantAbilityFragment* GrantFrag = Item->FindFragment<UTDWGrantAbilityFragment>())
		{
			for (const FTDWAbilityGrant& Grant : GrantFrag->AbilitiesToGrant)
				ASC->GrantAndBindAbility(Grant.AbilityClass, Grant.InputAction, Grant.DefaultKey);
		}
	}

	if (const UTDWMeshFragment* MeshFrag = Item->FindFragment<UTDWMeshFragment>())
	{
		if (OwnerChar)
		{
			UStaticMeshComponent* StaticMesh = NewObject<UStaticMeshComponent>(OwnerChar);
			StaticMesh->SetStaticMesh(MeshFrag->Mesh.Get());
			StaticMesh->SetWorldScale3D(FVector(MeshFrag->Scale));
			StaticMesh->SetMobility(EComponentMobility::Movable);
			StaticMesh->RegisterComponent();
			StaticMesh->AttachToComponent(OwnerChar->GetMesh(),
				FAttachmentTransformRules::KeepRelativeTransform, MeshFrag->Socket);
			ItemMeshes.Add(StaticMesh);
		}
	}
}

void UTDWControllerInventory::DropItem(int ItemIndex)
{
	if (!Items.IsValidIndex(ItemIndex) || !OwnerPC) return;

	APawn* Pawn = OwnerPC->GetPawn();
	if (!Pawn) return;

	UTDWItemDefinition* ItemDef = Items[ItemIndex].ItemDefinition;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ATDWPickUpActor* Spawned = GetWorld()->SpawnActor<ATDWPickUpActor>(
		ATDWPickUpActor::StaticClass(), Pawn->GetActorTransform(), SpawnParams);
	if (Spawned)
		Spawned->ItemDefinition = ItemDef;

	Items.RemoveAt(ItemIndex);

	ACharacter* OwnerChar = Cast<ACharacter>(Pawn);
	UTDWAbilitySystem* ASC = OwnerChar
		? Cast<UTDWAbilitySystem>(Cast<IAbilitySystemInterface>(OwnerChar)->GetAbilitySystemComponent())
		: nullptr;

	if (ASC)
	{
		if (const UTDWGrantAbilityFragment* GrantFrag = ItemDef->FindFragment<UTDWGrantAbilityFragment>())
		{
			for (const FTDWAbilityGrant& Grant : GrantFrag->AbilitiesToGrant)
				ASC->RemoveAndUnbindAbility(Grant.AbilityClass);
		}
	}

	if (const UTDWMeshFragment* MeshFrag = ItemDef->FindFragment<UTDWMeshFragment>())
	{
		for (int i = 0; i < ItemMeshes.Num(); i++)
		{
			if (ItemMeshes[i]->GetStaticMesh() == MeshFrag->Mesh.Get())
			{
				ItemMeshes[i]->DestroyComponent();
				ItemMeshes.RemoveAt(i);
				break;
			}
		}
	}
}

bool UTDWControllerInventory::EvaluateAttachment(int ItemIndex, int AttachmentSlot, UTDWItemDefinition* Attachment) const
{
	if (!Items.IsValidIndex(ItemIndex))
		return false;
	if (!Attachment)
		return false;

	const FTDWItemInstance& Item = Items[ItemIndex];
	if (!Item.Attachments.IsValidIndex(AttachmentSlot))
		return false;

	for (const FTDWAttachDefinition& Def : Item.ItemDefinition->AttachableDefinitions)
		if (Def.Type == Attachment->ItemType)
			return true;

	return false;
}

void UTDWControllerInventory::BeginPlay()
{
	Super::BeginPlay();
	OwnerPC = Cast<APlayerController>(GetOwner());
	ensure(OwnerPC);
}
