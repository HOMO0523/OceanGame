#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanBuildGridComponent.h"
#include "OceanPrototype/OceanBuildModuleDefinition.h"
#include "OceanPrototype/OceanBuildPlacementTypes.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

namespace
{
UWorld* CreatePlacementQueryTestWorld()
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

void DestroyPlacementQueryTestWorld(UWorld* World)
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

UOceanInventoryComponent* AddPlacementQueryInventory(AActor* Actor)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>(Actor);
	Actor->AddInstanceComponent(Inventory);
	Inventory->RegisterComponent();
	return Inventory;
}

UOceanBuildComponent* AddPlacementQueryBuild(AActor* Actor)
{
	UOceanBuildComponent* Build = NewObject<UOceanBuildComponent>(Actor);
	Actor->AddInstanceComponent(Build);
	Build->RegisterComponent();
	return Build;
}

UOceanBuildModuleDefinition* CreatePlacementQueryDeckDefinition()
{
	UOceanBuildModuleDefinition* DeckDefinition = NewObject<UOceanBuildModuleDefinition>();
	DeckDefinition->DisplayName = NSLOCTEXT("OceanTests", "PlacementQueryDeckDefinition", "Deck");
	DeckDefinition->FootprintSize = FIntPoint(1, 1);
	DeckDefinition->BuildCost = { { EOceanResourceType::Wood, 2 } };
	DeckDefinition->bRequiresAdjacency = true;
	return DeckDefinition;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPPlacementQueryTest, "Ocean.MVP.Build.PlacementQuery", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPPlacementQueryTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreatePlacementQueryTestWorld();
	TestNotNull(TEXT("[TDD] OceanPlacementQuery_TestWorld"), World);
	if (!World)
	{
		return false;
	}

	AOceanFloatingPlatform* Platform = World->SpawnActor<AOceanFloatingPlatform>(FVector::ZeroVector, FRotator::ZeroRotator);
	Platform->SetInitialCoreSize(FIntPoint(1, 1));
	Platform->InitializeCorePlatform();
	UOceanBuildGridComponent* Grid = Platform->GetBuildGrid();
	TestNotNull(TEXT("[TDD] OceanPlacementQuery_BuildGrid"), Grid);

	UOceanBuildModuleDefinition* DeckDefinition = CreatePlacementQueryDeckDefinition();

	AActor* Builder = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UOceanInventoryComponent* Inventory = AddPlacementQueryInventory(Builder);
	TestTrue(TEXT("[TDD] OceanPlacementQuery_AddWood"), Inventory->AddResource({ EOceanResourceType::Wood, 2 }));

	UOceanBuildComponent* Build = AddPlacementQueryBuild(Builder);
	Build->SetTargetPlatform(Platform);
	Build->SetSelectedModule(DeckDefinition);
	Build->SetBuildModeActive(true);

	const FOceanPlacementQueryResult OccupiedResult = Build->QuerySelectedModulePlacement(Grid->CellToWorld(FIntPoint(0, 0)));
	TestFalse(TEXT("[TDD] OceanPlacementQuery_OccupiedRejected"), OccupiedResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_OccupiedReason"), OccupiedResult.FailureReason, EOceanPlacementFailureReason::OccupiedCell);

	const FOceanPlacementQueryResult DetachedResult = Build->QuerySelectedModulePlacement(Grid->CellToWorld(FIntPoint(5, 5)));
	TestFalse(TEXT("[TDD] OceanPlacementQuery_DetachedRejected"), DetachedResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_DetachedReason"), DetachedResult.FailureReason, EOceanPlacementFailureReason::DetachedFromPlatform);

	const FOceanPlacementQueryResult AdjacentResult = Build->QuerySelectedModulePlacement(Grid->CellToWorld(FIntPoint(1, 0)));
	TestTrue(TEXT("[TDD] OceanPlacementQuery_AdjacentAccepted"), AdjacentResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_AdjacentReason"), AdjacentResult.FailureReason, EOceanPlacementFailureReason::None);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_AdjacentAnchor"), AdjacentResult.AnchorCell, FIntPoint(1, 0));

	DestroyPlacementQueryTestWorld(World);
	return true;
}

#endif
