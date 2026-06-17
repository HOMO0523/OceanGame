#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanBuildGridComponent.h"
#include "OceanPrototype/OceanBuildModuleActor.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanResourceField.h"
#include "OceanPrototype/OceanResourceNode.h"
#include "BuoyancyComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PCGComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBuildGridWorldToCellTest, "Ocean.Build.Grid.WorldToCell", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBuildGridWorldToCellTest::RunTest(const FString& Parameters)
{
	UOceanBuildGridComponent* Grid = NewObject<UOceanBuildGridComponent>();
	Grid->SetCellSize(100.0f);
	Grid->SetGridOrigin(FVector::ZeroVector);

	TestEqual(TEXT("[TDD] OceanGrid_WorldToCell_X"), Grid->WorldToCell(FVector(149.0f, 151.0f, 0.0f)).X, 1);
	TestEqual(TEXT("[TDD] OceanGrid_WorldToCell_Y"), Grid->WorldToCell(FVector(149.0f, 151.0f, 0.0f)).Y, 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBuildGridFootprintRotationTest, "Ocean.Build.Grid.FootprintRotation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBuildGridFootprintRotationTest::RunTest(const FString& Parameters)
{
	const TArray<FIntPoint> Footprint = UOceanBuildGridComponent::BuildFootprint(FIntPoint(0, 0), FIntPoint(2, 1), 1);

	TestEqual(TEXT("[TDD] OceanGrid_RotatedFootprint_Count"), Footprint.Num(), 2);
	TestTrue(TEXT("[TDD] OceanGrid_RotatedFootprint_Origin"), Footprint.Contains(FIntPoint(0, 0)));
	TestTrue(TEXT("[TDD] OceanGrid_RotatedFootprint_Vertical"), Footprint.Contains(FIntPoint(0, 1)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBuildGridOverlapTest, "Ocean.Build.Grid.OverlapRejected", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBuildGridOverlapTest::RunTest(const FString& Parameters)
{
	UOceanBuildGridComponent* Grid = NewObject<UOceanBuildGridComponent>();
	Grid->ReserveFootprint({ FIntPoint(0, 0) }, TEXT("Core"));

	TestFalse(TEXT("[TDD] OceanGrid_OverlapRejected"), Grid->CanPlaceFootprint({ FIntPoint(0, 0) }, false));
	TestTrue(TEXT("[TDD] OceanGrid_FreeCellAccepted"), Grid->CanPlaceFootprint({ FIntPoint(1, 0) }, false));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBuildGridAdjacencyTest, "Ocean.Build.Grid.AdjacencyRequired", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBuildGridAdjacencyTest::RunTest(const FString& Parameters)
{
	UOceanBuildGridComponent* Grid = NewObject<UOceanBuildGridComponent>();
	Grid->ReserveFootprint({ FIntPoint(0, 0) }, TEXT("Core"));

	TestTrue(TEXT("[TDD] OceanGrid_AdjacentAccepted"), Grid->CanPlaceFootprint({ FIntPoint(1, 0) }, true));
	TestFalse(TEXT("[TDD] OceanGrid_DetachedRejected"), Grid->CanPlaceFootprint({ FIntPoint(5, 5) }, true));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBuildGridWaterEdgeTest, "Ocean.Build.Grid.WaterEdgeCells", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBuildGridWaterEdgeTest::RunTest(const FString& Parameters)
{
	UOceanBuildGridComponent* Grid = NewObject<UOceanBuildGridComponent>();
	Grid->ReserveFootprint(UOceanBuildGridComponent::BuildFootprint(FIntPoint(0, 0), FIntPoint(3, 3), 0), TEXT("TestPlatform"));

	TestFalse(TEXT("[TDD] OceanGrid_WaterEdge_CenterRejected"), Grid->IsWaterAdjacentCell(FIntPoint(1, 1)));
	TestTrue(TEXT("[TDD] OceanGrid_WaterEdge_WestAccepted"), Grid->IsWaterAdjacentCell(FIntPoint(0, 1)));
	TestTrue(TEXT("[TDD] OceanGrid_WaterEdge_CornerAccepted"), Grid->IsWaterAdjacentCell(FIntPoint(0, 0)));
	TestFalse(TEXT("[TDD] OceanGrid_WaterEdge_UnoccupiedRejected"), Grid->IsWaterAdjacentCell(FIntPoint(4, 4)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanFloatingPlatformWaterEdgeTest, "Ocean.Build.Platform.WorldLocationWaterEdge", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanFloatingPlatformWaterEdgeTest::RunTest(const FString& Parameters)
{
	AOceanFloatingPlatform* Platform = NewObject<AOceanFloatingPlatform>();
	Platform->SetInitialCoreSize(FIntPoint(3, 3));
	Platform->InitializeCorePlatform();

	TestFalse(TEXT("[TDD] OceanPlatform_WaterEdge_CenterRejected"), Platform->IsWorldLocationAtWaterEdge(Platform->GetBuildGrid()->CellToWorld(FIntPoint(0, 0))));
	TestTrue(TEXT("[TDD] OceanPlatform_WaterEdge_NorthAccepted"), Platform->IsWorldLocationAtWaterEdge(Platform->GetBuildGrid()->CellToWorld(FIntPoint(0, 1))));

	Platform->SetInitialCoreSize(FIntPoint(2, 2));
	Platform->InitializeCorePlatform();
	TestTrue(TEXT("[TDD] OceanPlatform_WaterEdge_StarterBoatAccepted"), Platform->IsWorldLocationAtWaterEdge(Platform->GetBuildGrid()->CellToWorld(FIntPoint(0, 0))));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBuildModuleComponentTest, "Ocean.Build.Module.HasPlaceholderAndBuoyancy", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBuildModuleComponentTest::RunTest(const FString& Parameters)
{
	AOceanBuildModuleActor* Module = NewObject<AOceanBuildModuleActor>();

	TestNotNull(TEXT("[TDD] OceanModule_StaticMeshComponent"), Module->GetMeshComponent());
	TestNotNull(TEXT("[TDD] OceanModule_BuoyancyComponent"), Module->FindComponentByClass<UBuoyancyComponent>());
	TestNotNull(TEXT("[TDD] OceanModule_CubeFallbackMesh"), Module->GetMeshComponent()->GetStaticMesh().Get());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanResourceNodeComponentTest, "Ocean.Resources.Node.HasPlaceholderAndBuoyancy", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanResourceNodeComponentTest::RunTest(const FString& Parameters)
{
	AOceanResourceNode* ResourceNode = NewObject<AOceanResourceNode>();

	TestNotNull(TEXT("[TDD] OceanResource_StaticMeshComponent"), ResourceNode->GetMeshComponent());
	TestNotNull(TEXT("[TDD] OceanResource_BuoyancyComponent"), ResourceNode->FindComponentByClass<UBuoyancyComponent>());
	TestNotNull(TEXT("[TDD] OceanResource_CubeFallbackMesh"), ResourceNode->GetMeshComponent()->GetStaticMesh().Get());
	TestTrue(TEXT("[TDD] OceanResource_PhysicsEnabled"), ResourceNode->GetMeshComponent()->IsSimulatingPhysics());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanFloatingPlatformCoreTest, "Ocean.Build.Platform.ReservesCoreCells", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanFloatingPlatformCoreTest::RunTest(const FString& Parameters)
{
	AOceanFloatingPlatform* Platform = NewObject<AOceanFloatingPlatform>();
	Platform->SetInitialCoreSize(FIntPoint(2, 2));
	Platform->InitializeCorePlatform();

	TestNotNull(TEXT("[TDD] OceanPlatform_BuildGridComponent"), Platform->GetBuildGrid());
	TestEqual(TEXT("[TDD] OceanPlatform_CoreCellCount"), Platform->GetBuildGrid()->GetOccupiedCellCount(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanResourceFieldTest, "Ocean.Resources.Field.PCGAndExclusion", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanResourceFieldTest::RunTest(const FString& Parameters)
{
	AOceanResourceField* ResourceField = NewObject<AOceanResourceField>();
	ResourceField->SetFallbackResourceCount(16);
	ResourceField->SetFieldRadius(2000.0f);
	ResourceField->SetExclusionRadius(500.0f);

	const TArray<FVector> Locations = ResourceField->GenerateFallbackResourceLocations(FVector::ZeroVector);

	TestNotNull(TEXT("[TDD] OceanResourceField_PCGComponent"), ResourceField->GetPCGComponent());
	TestEqual(TEXT("[TDD] OceanResourceField_LocationCount"), Locations.Num(), 16);

	for (const FVector& Location : Locations)
	{
		TestTrue(TEXT("[TDD] OceanResourceField_OutsideExclusion"), FVector::Dist2D(Location, FVector::ZeroVector) >= 500.0f);
	}

	return true;
}

#endif
