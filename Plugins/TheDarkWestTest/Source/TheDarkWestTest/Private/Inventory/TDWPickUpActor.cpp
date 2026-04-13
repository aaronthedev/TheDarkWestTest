
#include "Inventory/TDWPickUpActor.h"

ATDWPickUpActor::ATDWPickUpActor()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;
	
	StaticMesh->SetMobility(EComponentMobility::Movable);
	
	StaticMesh->SetCanEverAffectNavigation(false);
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ATDWPickUpActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ItemDefinition)
	{
		Destroy();
		return;
	}
}

void ATDWPickUpActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (!ItemDefinition)
		return;
	
	const UTDWMeshFragment* MeshFrag = ItemDefinition->FindFragment<UTDWMeshFragment>();
	if (!MeshFrag)
		return;
	
	MeshFrag->LoadAssets();
	
	StaticMesh->SetStaticMesh(MeshFrag->Mesh.Get());
	StaticMesh->SetWorldScale3D(FVector(MeshFrag->Scale));
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	
}
