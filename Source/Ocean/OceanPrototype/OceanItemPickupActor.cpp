#include "OceanPrototype/OceanItemPickupActor.h"
#include "OceanPrototype/OceanItemDefinition.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

AOceanItemPickupActor::AOceanItemPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 30x30x30 cm cube placeholder. Scale 0.3 on a 100-unit cube mesh = 30cm.
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetEnableGravity(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
		MeshComponent->SetWorldScale3D(FVector(0.3f));
	}

	// Floating label above the pickup.
	LabelComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LabelComponent"));
	LabelComponent->SetupAttachment(MeshComponent);
	LabelComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	LabelComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 180.0f));
	LabelComponent->SetText(FText::FromString(TEXT("Item")));
	LabelComponent->SetTextRenderColor(FColor::White);
	LabelComponent->SetXScale(1.0f);
	LabelComponent->SetYScale(1.0f);
	LabelComponent->SetVisibility(true);

	// Sprite billboard so the pickup is visible even from far / without mesh art.
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	BillboardComponent->SetupAttachment(MeshComponent);
	BillboardComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f));
	BillboardComponent->SetHiddenInGame(false);

	// Interaction volume. Mirrors OceanResourceNode's pattern.
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(MeshComponent);
	InteractionSphere->InitSphereRadius(140.0f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AOceanItemPickupActor::InitializeFromDefinition(UOceanItemDefinition* Definition, int32 InQuantity)
{
	ItemDefinition = Definition;
	Quantity = FMath::Max(1, InQuantity);
	RefreshVisuals();
}

void AOceanItemPickupActor::RefreshVisuals()
{
	if (ItemDefinition)
	{
		LabelComponent->SetText(ItemDefinition->DisplayName.IsEmpty()
			? FText::FromString(TEXT("Item"))
			: ItemDefinition->DisplayName);
	}
	else
	{
		LabelComponent->SetText(FText::FromString(TEXT("Item")));
	}
}

FText AOceanItemPickupActor::GetOceanInteractionText_Implementation(AActor* Interactor) const
{
	if (ItemDefinition)
	{
		FText Name = ItemDefinition->DisplayName.IsEmpty()
			? FText::FromString(TEXT("Item"))
			: ItemDefinition->DisplayName;
		return FText::Format(FText::FromString(TEXT("F Pick up {0} x{1}")), Name, FText::AsNumber(Quantity));
	}
	return FText::FromString(TEXT("F Pick up Item"));
}

bool AOceanItemPickupActor::CanOceanInteract_Implementation(AActor* Interactor, FText& FailureReason) const
{
	if (!Interactor)
	{
		FailureReason = FText::FromString(TEXT("No interactor"));
		return false;
	}

	if (!ItemDefinition)
	{
		FailureReason = FText::FromString(TEXT("No item definition"));
		return false;
	}

	if (Quantity <= 0)
	{
		FailureReason = FText::FromString(TEXT("Pickup empty"));
		return false;
	}

	const UOceanInventoryComponent* Inventory = Interactor->FindComponentByClass<UOceanInventoryComponent>();
	if (!Inventory)
	{
		FailureReason = FText::FromString(TEXT("No inventory"));
		return false;
	}

	// The inventory's AddItem performs the full space check; we approximate by
	// verifying the inventory exists and has at least one free slot policy via
	// the slots accessor. A precise pre-check would require a CanAcceptItem API
	// on the inventory (not added to avoid modifying existing files). The actual
	// space validation happens in ExecuteOceanInteraction through AddItem, which
	// returns false when no space is available.
	return true;
}

bool AOceanItemPickupActor::ExecuteOceanInteraction_Implementation(AActor* Interactor, FText& OutMessage)
{
	if (!Interactor)
	{
		OutMessage = FText::FromString(TEXT("No interactor"));
		return false;
	}

	if (!ItemDefinition || Quantity <= 0)
	{
		OutMessage = FText::FromString(TEXT("Pickup empty"));
		return false;
	}

	UOceanInventoryComponent* Inventory = Interactor->FindComponentByClass<UOceanInventoryComponent>();
	if (!Inventory)
	{
		OutMessage = FText::FromString(TEXT("No inventory"));
		return false;
	}

	const int32 GrantedQuantity = FMath::Clamp(Quantity, 1, FMath::Max(1, ItemDefinition->MaxStack));
	const FOceanItemStack Stack = ItemDefinition->ToItemStack(GrantedQuantity);

	if (!Inventory->AddItem(Stack))
	{
		OutMessage = FText::FromString(TEXT("Inventory full"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("[TDD] OceanItemPickup picked up %s x%d"), *ItemDefinition->ItemId.ToString(), GrantedQuantity);

	const FText Name = ItemDefinition->DisplayName.IsEmpty()
		? FText::FromString(TEXT("Item"))
		: ItemDefinition->DisplayName;
	OutMessage = FText::Format(FText::FromString(TEXT("Picked up {0} x{1}")), Name, FText::AsNumber(GrantedQuantity));

	Destroy();
	return true;
}
