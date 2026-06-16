#include "OceanPrototype/OceanResourceNode.h"
#include "BuoyancyComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
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
	Amount = 0;
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

FText AOceanResourceNode::GetOceanInteractionText_Implementation(AActor* Interactor) const
{
	const UEnum* ResourceEnum = StaticEnum<EOceanResourceType>();
	const FText ResourceText = ResourceEnum
		? ResourceEnum->GetDisplayNameTextByValue(static_cast<int64>(ResourceType))
		: FText::FromString(TEXT("Resource"));

	return FText::Format(FText::FromString(TEXT("F 拾取 {0} x{1}")), ResourceText, FText::AsNumber(Amount));
}

bool AOceanResourceNode::CanOceanInteract_Implementation(AActor* Interactor, FText& FailureReason) const
{
	if (!Interactor)
	{
		FailureReason = FText::FromString(TEXT("没有交互者"));
		return false;
	}

	if (Amount <= 0)
	{
		FailureReason = FText::FromString(TEXT("资源已被拾取"));
		return false;
	}

	const UOceanInventoryComponent* Inventory = Interactor->FindComponentByClass<UOceanInventoryComponent>();
	if (!Inventory)
	{
		FailureReason = FText::FromString(TEXT("没有背包"));
		return false;
	}

	if (!Inventory->CanAcceptResource(GetResourceStack()))
	{
		FailureReason = FText::FromString(TEXT("背包已满"));
		return false;
	}

	return true;
}

bool AOceanResourceNode::ExecuteOceanInteraction_Implementation(AActor* Interactor, FText& OutMessage)
{
	if (!Interactor)
	{
		OutMessage = FText::FromString(TEXT("没有交互者"));
		return false;
	}

	FText FailureReason;
	if (!CanOceanInteract(Interactor, FailureReason))
	{
		OutMessage = FailureReason;
		return false;
	}

	UOceanInventoryComponent* Inventory = Interactor->FindComponentByClass<UOceanInventoryComponent>();
	if (!Inventory)
	{
		OutMessage = FText::FromString(TEXT("没有背包"));
		return false;
	}

	const FOceanResourceStack Stack = GetResourceStack();
	if (!Inventory->AddResource(Stack))
	{
		OutMessage = FText::FromString(TEXT("背包已满"));
		return false;
	}

	Collect();
	OutMessage = FText::FromString(TEXT("已拾取资源"));
	return true;
}
