#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInteractableInterface.h"
#include "OceanPrototype/OceanInteractionComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanResourceNode.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/UnrealType.h"

namespace
{
UWorld* CreateInteractionTestWorld()
{
	UGameInstance* TestGameInstance = NewObject<UGameInstance>(GEngine);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (World)
	{
		World->SetShouldTick(false);
		World->AddToRoot();

		FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
		WorldContext.OwningGameInstance = TestGameInstance;
		World->SetGameInstance(TestGameInstance);
		WorldContext.SetCurrentWorld(World);

		if (TestGameInstance)
		{
			TestGameInstance->Init();
		}
	}

	return World;
}

void DestroyInteractionTestWorld(UWorld* World)
{
	if (World)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			GameInstance->Shutdown();
		}

		GEngine->DestroyWorldContext(World);
		World->RemoveFromRoot();
		World->DestroyWorld(false);
	}
}

void SetResourceTypeForTest(AOceanResourceNode* Node, EOceanResourceType ResourceType)
{
	if (!Node)
	{
		return;
	}

	if (FEnumProperty* ResourceTypeProperty = FindFProperty<FEnumProperty>(AOceanResourceNode::StaticClass(), TEXT("ResourceType")))
	{
		void* Value = ResourceTypeProperty->ContainerPtrToValuePtr<void>(Node);
		ResourceTypeProperty->GetUnderlyingProperty()->SetIntPropertyValue(Value, static_cast<int64>(ResourceType));
	}
}

UOceanInventoryComponent* AddInventoryForTest(AActor* Actor)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>(Actor);
	Actor->AddInstanceComponent(Inventory);
	Inventory->RegisterComponent();
	return Inventory;
}

UOceanInteractionComponent* AddInteractionForTest(AActor* Actor, float Radius)
{
	UOceanInteractionComponent* Interaction = NewObject<UOceanInteractionComponent>(Actor);
	Actor->AddInstanceComponent(Interaction);
	Interaction->RegisterComponent();
	Interaction->SetInteractionRadius(Radius);
	return Interaction;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPResourcePickupTest, "Ocean.MVP.Interaction.ResourcePickup", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPResourcePickupTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	AOceanResourceNode* Node = NewObject<AOceanResourceNode>();

	const FOceanResourceStack Stack = Node->Collect();
	TestTrue(TEXT("Collected stack has positive amount"), Stack.Amount > 0);
	TestTrue(TEXT("Inventory accepts collected stack"), Inventory->AddResource(Stack));
	TestEqual(TEXT("Inventory contains collected resource"), Inventory->GetResourceAmount(Stack.ResourceType), Stack.Amount);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPResourceInteractionTest, "Ocean.MVP.Interaction.ResourceNodeExecutesPickup", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPResourceInteractionTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateInteractionTestWorld();
	TestNotNull(TEXT("[TDD] OceanInteraction_TestWorld"), World);
	if (!World)
	{
		return false;
	}

	AActor* Interactor = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UOceanInventoryComponent* Inventory = AddInventoryForTest(Interactor);

	AOceanResourceNode* Node = World->SpawnActor<AOceanResourceNode>(FVector(100.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	const FOceanResourceStack Stack = Node->GetResourceStack();

	FText Message;
	TestTrue(TEXT("[TDD] OceanInteraction_NodeImplementsInterface"), Node->GetClass()->ImplementsInterface(UOceanInteractableInterface::StaticClass()));
	TestTrue(TEXT("[TDD] OceanInventory_CanAcceptCollectedStack"), Inventory->CanAcceptResource(Stack));
	TestTrue(TEXT("[TDD] OceanInteraction_ExecutePickup"), Node->ExecuteOceanInteraction(Interactor, Message));
	TestEqual(TEXT("[TDD] OceanInteraction_Message"), Message.ToString(), FString(TEXT("已拾取资源")));
	TestEqual(TEXT("[TDD] OceanInteraction_InventoryAmount"), Inventory->GetResourceAmount(Stack.ResourceType), Stack.Amount);

	DestroyInteractionTestWorld(World);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInteractionNoValidTargetTest, "Ocean.MVP.Interaction.NoValidTarget", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInteractionNoValidTargetTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateInteractionTestWorld();
	TestNotNull(TEXT("[TDD] OceanInteraction_TestWorld"), World);
	if (!World)
	{
		return false;
	}

	AActor* Interactor = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UOceanInventoryComponent* Inventory = AddInventoryForTest(Interactor);
	Inventory->SetMaxSlots(1);
	TestTrue(TEXT("[TDD] OceanInteraction_FillInventory"), Inventory->AddResource({EOceanResourceType::Wood, 1}));
	UOceanInteractionComponent* Interaction = AddInteractionForTest(Interactor, 500.0f);

	AOceanResourceNode* BlockedNode = World->SpawnActor<AOceanResourceNode>(FVector(100.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	SetResourceTypeForTest(BlockedNode, EOceanResourceType::Scrap);
	World->Tick(ELevelTick::LEVELTICK_All, 0.0f);

	FText Message;
	TestNull(TEXT("[TDD] OceanInteraction_NoValidTarget"), Interaction->FindBestInteractable());
	TestFalse(TEXT("[TDD] OceanInteraction_NoValidTryInteract"), Interaction->TryInteract(Message));
	TestEqual(TEXT("[TDD] OceanInteraction_NoTargetMessage"), Message.ToString(), FString(TEXT("没有可交互目标")));

	DestroyInteractionTestWorld(World);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInteractionSkipsNearUnpickableTest, "Ocean.MVP.Interaction.SkipsNearUnpickable", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInteractionSkipsNearUnpickableTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateInteractionTestWorld();
	TestNotNull(TEXT("[TDD] OceanInteraction_TestWorld"), World);
	if (!World)
	{
		return false;
	}

	AActor* Interactor = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UOceanInventoryComponent* Inventory = AddInventoryForTest(Interactor);
	Inventory->SetMaxSlots(1);
	TestTrue(TEXT("[TDD] OceanInteraction_ExistingWoodStack"), Inventory->AddResource({EOceanResourceType::Wood, 1}));
	UOceanInteractionComponent* Interaction = AddInteractionForTest(Interactor, 500.0f);

	AOceanResourceNode* NearBlockedNode = World->SpawnActor<AOceanResourceNode>(FVector(100.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	SetResourceTypeForTest(NearBlockedNode, EOceanResourceType::Scrap);
	AOceanResourceNode* FarValidNode = World->SpawnActor<AOceanResourceNode>(FVector(300.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	World->Tick(ELevelTick::LEVELTICK_All, 0.0f);

	FText Message;
	TestEqual(TEXT("[TDD] OceanInteraction_SkipsNearFullInventoryTarget"), Interaction->FindBestInteractable(), static_cast<AActor*>(FarValidNode));
	TestTrue(TEXT("[TDD] OceanInteraction_InteractFarValidTarget"), Interaction->TryInteract(Message));
	TestEqual(TEXT("[TDD] OceanInteraction_FarPickupMessage"), Message.ToString(), FString(TEXT("已拾取资源")));
	TestEqual(TEXT("[TDD] OceanInteraction_WoodStackAfterPickup"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 2);

	DestroyInteractionTestWorld(World);
	return true;
}

#endif
