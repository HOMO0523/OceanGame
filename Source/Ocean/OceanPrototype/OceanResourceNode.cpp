#include "OceanPrototype/OceanResourceNode.h"
#include "BuoyancyComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AOceanResourceNode::AOceanResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
		MeshComponent->SetWorldScale3D(FVector(0.5f));
	}

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(MeshComponent);
	InteractionSphere->InitSphereRadius(CollectionRadius);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);

	BuoyancyComponent = CreateDefaultSubobject<UBuoyancyComponent>(TEXT("BuoyancyComponent"));
	BuoyancyComponent->AddCustomPontoon(60.0f, FVector::ZeroVector);
}

FOceanResourceStack AOceanResourceNode::GetResourceStack() const
{
	FOceanResourceStack Stack;
	Stack.ResourceType = ResourceType;
	Stack.Amount = Amount;
	return Stack;
}

FOceanResourceStack AOceanResourceNode::Collect()
{
	const FOceanResourceStack Stack = GetResourceStack();
	Destroy();
	return Stack;
}

void AOceanResourceNode::SetPlaceholderMesh(UStaticMesh* NewMesh)
{
	if (NewMesh)
	{
		MeshComponent->SetStaticMesh(NewMesh);
	}
}
