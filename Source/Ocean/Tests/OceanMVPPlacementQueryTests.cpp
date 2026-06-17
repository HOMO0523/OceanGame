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

	UOceanBuildModuleDefinition* DeckDefinition = CreatePlacementQueryDeckDefinition();
	AActor* Builder = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("[TDD] OceanPlacementQuery_Builder"), Builder);
	if (!Builder)
	{
		DestroyPlacementQueryTestWorld(World);
		return false;
	}

	UOceanBuildComponent* Build = AddPlacementQueryBuild(Builder);
	Build->SetSelectedModule(DeckDefinition);
	Build->SetBuildModeActive(true);

	const FOceanPlacementQueryResult NoTargetResult = Build->QuerySelectedModulePlacement(FVector::ZeroVector);
	TestFalse(TEXT("[TDD] OceanPlacementQuery_NoTargetRejected"), NoTargetResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_NoTargetReason"), NoTargetResult.FailureReason, EOceanPlacementFailureReason::NoTargetPlatform);

	AOceanFloatingPlatform* Platform = World->SpawnActor<AOceanFloatingPlatform>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("[TDD] OceanPlacementQuery_Platform"), Platform);
	if (!Platform)
	{
		DestroyPlacementQueryTestWorld(World);
		return false;
	}

	Platform->SetInitialCoreSize(FIntPoint(1, 1));
	Platform->InitializeCorePlatform();
	UOceanBuildGridComponent* Grid = Platform->GetBuildGrid();
	TestNotNull(TEXT("[TDD] OceanPlacementQuery_BuildGrid"), Grid);
	if (!Grid)
	{
		DestroyPlacementQueryTestWorld(World);
		return false;
	}

	UOceanInventoryComponent* Inventory = AddPlacementQueryInventory(Builder);
	TestTrue(TEXT("[TDD] OceanPlacementQuery_AddWood"), Inventory->AddResource({ EOceanResourceType::Wood, 2 }));

	Build->SetTargetPlatform(Platform);
	Build->SetSelectedModule(DeckDefinition);
	Build->SetBuildModeActive(true);

	const FVector AdjacentLocation = Grid->CellToWorld(FIntPoint(1, 0));

	Build->SetBuildModeActive(false);
	const FOceanPlacementQueryResult NoBuildModeResult = Build->QuerySelectedModulePlacement(AdjacentLocation);
	TestFalse(TEXT("[TDD] OceanPlacementQuery_NoBuildModeRejected"), NoBuildModeResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_NoBuildModeReason"), NoBuildModeResult.FailureReason, EOceanPlacementFailureReason::NoBuildMode);

	Build->SetBuildModeActive(true);
	Build->SetSelectedModule(nullptr);
	const FOceanPlacementQueryResult NoSelectedModuleResult = Build->QuerySelectedModulePlacement(AdjacentLocation);
	TestFalse(TEXT("[TDD] OceanPlacementQuery_NoSelectedModuleRejected"), NoSelectedModuleResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_NoSelectedModuleReason"), NoSelectedModuleResult.FailureReason, EOceanPlacementFailureReason::NoSelectedModule);

	Build->SetSelectedModule(DeckDefinition);

	AActor* PoorBuilder = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	TestNotNull(TEXT("[TDD] OceanPlacementQuery_PoorBuilder"), PoorBuilder);
	if (!PoorBuilder)
	{
		DestroyPlacementQueryTestWorld(World);
		return false;
	}

	UOceanInventoryComponent* PoorInventory = AddPlacementQueryInventory(PoorBuilder);
	TestTrue(TEXT("[TDD] OceanPlacementQuery_AddInsufficientWood"), PoorInventory->AddResource({ EOceanResourceType::Wood, 1 }));
	UOceanBuildComponent* PoorBuild = AddPlacementQueryBuild(PoorBuilder);
	PoorBuild->SetTargetPlatform(Platform);
	PoorBuild->SetSelectedModule(DeckDefinition);
	PoorBuild->SetBuildModeActive(true);

	const FOceanPlacementQueryResult InsufficientResourcesResult = PoorBuild->QuerySelectedModulePlacement(AdjacentLocation);
	TestFalse(TEXT("[TDD] OceanPlacementQuery_InsufficientResourcesRejected"), InsufficientResourcesResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_InsufficientResourcesReason"), InsufficientResourcesResult.FailureReason, EOceanPlacementFailureReason::InsufficientResources);

	const FOceanPlacementQueryResult OccupiedResult = Build->QuerySelectedModulePlacement(Grid->CellToWorld(FIntPoint(0, 0)));
	TestFalse(TEXT("[TDD] OceanPlacementQuery_OccupiedRejected"), OccupiedResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_OccupiedReason"), OccupiedResult.FailureReason, EOceanPlacementFailureReason::OccupiedCell);

	const FOceanPlacementQueryResult DetachedResult = Build->QuerySelectedModulePlacement(Grid->CellToWorld(FIntPoint(5, 5)));
	TestFalse(TEXT("[TDD] OceanPlacementQuery_DetachedRejected"), DetachedResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_DetachedReason"), DetachedResult.FailureReason, EOceanPlacementFailureReason::DetachedFromPlatform);

	const FOceanPlacementQueryResult AdjacentResult = Build->QuerySelectedModulePlacement(AdjacentLocation);
	TestTrue(TEXT("[TDD] OceanPlacementQuery_AdjacentAccepted"), AdjacentResult.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_AdjacentReason"), AdjacentResult.FailureReason, EOceanPlacementFailureReason::None);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_AdjacentAnchor"), AdjacentResult.AnchorCell, FIntPoint(1, 0));

	DestroyPlacementQueryTestWorld(World);
	return true;
}

#endif
