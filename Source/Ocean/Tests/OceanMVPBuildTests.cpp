#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanBuildGridComponent.h"
#include "OceanPrototype/OceanBuildModuleActor.h"
#include "OceanPrototype/OceanBuildModuleDefinition.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "BuoyancyComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

namespace
{
UWorld* CreateBuildTestWorld()
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

void DestroyBuildTestWorld(UWorld* World)
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

UOceanInventoryComponent* AddInventoryForBuildTest(AActor* Actor)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>(Actor);
	Actor->AddInstanceComponent(Inventory);
	Inventory->RegisterComponent();
	return Inventory;
}

UOceanBuildComponent* AddBuildForTest(AActor* Actor)
{
	UOceanBuildComponent* Build = NewObject<UOceanBuildComponent>(Actor);
	Actor->AddInstanceComponent(Build);
	Build->RegisterComponent();
	return Build;
}

UOceanBuildModuleDefinition* CreateDeckDefinitionForTest()
{
	UOceanBuildModuleDefinition* DeckDefinition = NewObject<UOceanBuildModuleDefinition>();
	DeckDefinition->DisplayName = NSLOCTEXT("OceanTests", "DeckDefinition", "Deck");
	DeckDefinition->FootprintSize = FIntPoint(1, 1);
	DeckDefinition->BuildCost = { { EOceanResourceType::Wood, 2 } };
	DeckDefinition->bRequiresAdjacency = true;
	return DeckDefinition;
}

AOceanFloatingPlatform* CreateOneCellPlatformForTest(UWorld* World)
{
	AOceanFloatingPlatform* Platform = World->SpawnActor<AOceanFloatingPlatform>(FVector::ZeroVector, FRotator::ZeroRotator);
	Platform->SetInitialCoreSize(FIntPoint(1, 1));
	Platform->InitializeCorePlatform();
	return Platform;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPDeckPlacementTest, "Ocean.MVP.Build.DeckPlacement", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPDeckPlacementTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateBuildTestWorld();
	TestNotNull(TEXT("[TDD] OceanBuild_TestWorld"), World);
	if (!World)
	{
		return false;
	}

	AActor* Builder = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UOceanInventoryComponent* Inventory = AddInventoryForBuildTest(Builder);
	TestTrue(TEXT("[TDD] OceanBuild_AddStartingWood"), Inventory->AddResource({ EOceanResourceType::Wood, 3 }));

	AOceanFloatingPlatform* Platform = CreateOneCellPlatformForTest(World);
	UOceanBuildGridComponent* Grid = Platform->GetBuildGrid();
	UOceanBuildModuleDefinition* DeckDefinition = CreateDeckDefinitionForTest();

	UOceanBuildComponent* Build = AddBuildForTest(Builder);
	Build->SetTargetPlatform(Platform);
	Build->SetSelectedModule(DeckDefinition);
	Build->SetBuildModeActive(true);

	const FIntPoint DeckCell(1, 0);
	const FVector PlacementLocation = Grid->CellToWorld(DeckCell);

	FText Message;
	TestTrue(TEXT("[TDD] OceanBuild_PlaceAdjacentDeck"), Build->TryPlaceSelectedModuleAtWorld(PlacementLocation, Message));
	TestEqual(TEXT("[TDD] OceanBuild_WoodAfterPlacement"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 1);
	TestTrue(TEXT("[TDD] OceanBuild_ReservedDeckCell"), Grid->IsCellOccupied(DeckCell));

	AOceanBuildModuleActor* PlacedModule = Build->GetLastPlacedModuleActor();
	TestNotNull(TEXT("[TDD] OceanBuild_SpawnedModule"), PlacedModule);
	if (PlacedModule)
	{
		TestEqual(TEXT("[TDD] OceanBuild_PlacedModuleCellCount"), PlacedModule->GetOccupiedCells().Num(), 1);
		TestTrue(TEXT("[TDD] OceanBuild_PlacedModuleConfiguredCell"), PlacedModule->GetOccupiedCells().Contains(DeckCell));
		TestNotNull(TEXT("[TDD] OceanBuild_PlacedModuleBuoyancy"), PlacedModule->FindComponentByClass<UBuoyancyComponent>());
	}

	FText FailedMessage;
	TestFalse(TEXT("[TDD] OceanBuild_FailsSecondDetachedPlacement"), Build->TryPlaceSelectedModuleAtWorld(Grid->CellToWorld(FIntPoint(5, 5)), FailedMessage));
	TestNull(TEXT("[TDD] OceanBuild_LastPlacedClearedAfterFailedPlacement"), Build->GetLastPlacedModuleActor());

	DestroyBuildTestWorld(World);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPBuildFailureCasesTest, "Ocean.MVP.Build.FailureCases", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPBuildFailureCasesTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateBuildTestWorld();
	TestNotNull(TEXT("[TDD] OceanBuild_TestWorld"), World);
	if (!World)
	{
		return false;
	}

	AActor* Builder = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UOceanInventoryComponent* Inventory = AddInventoryForBuildTest(Builder);
	TestTrue(TEXT("[TDD] OceanBuild_AddFailureCaseWood"), Inventory->AddResource({ EOceanResourceType::Wood, 4 }));

	AOceanFloatingPlatform* Platform = CreateOneCellPlatformForTest(World);
	UOceanBuildGridComponent* Grid = Platform->GetBuildGrid();
	UOceanBuildModuleDefinition* DeckDefinition = CreateDeckDefinitionForTest();
	UOceanBuildComponent* Build = AddBuildForTest(Builder);

	const FVector AdjacentLocation = Grid->CellToWorld(FIntPoint(1, 0));
	const FVector OccupiedLocation = Grid->CellToWorld(FIntPoint(0, 0));
	const FVector DetachedLocation = Grid->CellToWorld(FIntPoint(5, 5));
	FText Message;

	Build->SetTargetPlatform(Platform);
	Build->SetSelectedModule(DeckDefinition);
	Build->SetBuildModeActive(false);
	TestFalse(TEXT("[TDD] OceanBuild_FailsWithoutBuildMode"), Build->TryPlaceSelectedModuleAtWorld(AdjacentLocation, Message));
	TestEqual(TEXT("[TDD] OceanBuild_WoodUnchangedWithoutBuildMode"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 4);

	Build->SetBuildModeActive(true);
	Build->SetSelectedModule(nullptr);
	TestFalse(TEXT("[TDD] OceanBuild_FailsWithoutSelectedModule"), Build->TryPlaceSelectedModuleAtWorld(AdjacentLocation, Message));

	Build->SetSelectedModule(DeckDefinition);
	Build->SetTargetPlatform(nullptr);
	TestFalse(TEXT("[TDD] OceanBuild_FailsWithoutTargetPlatform"), Build->TryPlaceSelectedModuleAtWorld(AdjacentLocation, Message));

	Build->SetTargetPlatform(Platform);
	TestTrue(TEXT("[TDD] OceanBuild_RemoveWoodForUnaffordableCase"), Inventory->TrySpend({ { EOceanResourceType::Wood, 4 } }));
	TestFalse(TEXT("[TDD] OceanBuild_FailsWhenUnaffordable"), Build->TryPlaceSelectedModuleAtWorld(AdjacentLocation, Message));
	TestFalse(TEXT("[TDD] OceanBuild_DoesNotReserveUnaffordableCell"), Grid->IsCellOccupied(FIntPoint(1, 0)));

	TestTrue(TEXT("[TDD] OceanBuild_AddWoodForInvalidPlacementCases"), Inventory->AddResource({ EOceanResourceType::Wood, 4 }));
	TestFalse(TEXT("[TDD] OceanBuild_FailsOnOccupiedCell"), Build->TryPlaceSelectedModuleAtWorld(OccupiedLocation, Message));
	TestEqual(TEXT("[TDD] OceanBuild_WoodUnchangedOnOccupiedCell"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 4);

	TestFalse(TEXT("[TDD] OceanBuild_FailsOnDetachedCell"), Build->TryPlaceSelectedModuleAtWorld(DetachedLocation, Message));
	TestEqual(TEXT("[TDD] OceanBuild_WoodUnchangedOnDetachedCell"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 4);

	Platform->Destroy();
	TestFalse(TEXT("[TDD] OceanBuild_FailsWithDestroyedTargetPlatform"), Build->TryPlaceSelectedModuleAtWorld(AdjacentLocation, Message));
	TestEqual(TEXT("[TDD] OceanBuild_WoodUnchangedWithDestroyedTargetPlatform"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 4);

	DestroyBuildTestWorld(World);
	return true;
}

#endif
