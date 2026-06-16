#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryStackingTest, "Ocean.MVP.Inventory.Stacking", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryStackingTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	Inventory->SetMaxSlots(2);

	TestFalse(TEXT("Zero amount resource rejected"), Inventory->AddResource({EOceanResourceType::Wood, 0}));
	TestFalse(TEXT("Negative amount resource rejected"), Inventory->AddResource({EOceanResourceType::Wood, -1}));
	TestTrue(TEXT("Add wood succeeds"), Inventory->AddResource({EOceanResourceType::Wood, 4}));
	TestTrue(TEXT("Add more wood stacks"), Inventory->AddResource({EOceanResourceType::Wood, 3}));
	TestEqual(TEXT("Wood amount stacked"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 7);

	TestTrue(TEXT("Add water uses second slot"), Inventory->AddResource({EOceanResourceType::Water, 1}));
	TestFalse(TEXT("Third type rejected when full"), Inventory->AddResource({EOceanResourceType::Food, 1}));

	const TArray<FOceanResourceStack> Cost = {{EOceanResourceType::Wood, 5}};
	TestTrue(TEXT("Can afford wood cost"), Inventory->CanAfford(Cost));
	TestTrue(TEXT("Spend wood succeeds"), Inventory->TrySpend(Cost));
	TestEqual(TEXT("Wood after spend"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 2);

	const TArray<FOceanResourceStack> TooExpensiveCost = {{EOceanResourceType::Wood, 3}};
	TestFalse(TEXT("Cannot afford insufficient wood cost"), Inventory->CanAfford(TooExpensiveCost));
	TestFalse(TEXT("Spend insufficient wood fails"), Inventory->TrySpend(TooExpensiveCost));
	TestEqual(TEXT("Wood unchanged after failed spend"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 2);

	const TArray<FOceanResourceStack> ExactRemainingCost = {{EOceanResourceType::Wood, 2}};
	TestTrue(TEXT("Exact spend succeeds"), Inventory->TrySpend(ExactRemainingCost));
	TestEqual(TEXT("Exact spend removes stack amount"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryDuplicateCostAtomicityTest, "Ocean.MVP.Inventory.DuplicateCostAtomicity", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryDuplicateCostAtomicityTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	Inventory->SetMaxSlots(2);
	TestTrue(TEXT("Add starting wood succeeds"), Inventory->AddResource({EOceanResourceType::Wood, 6}));

	const TArray<FOceanResourceStack> DuplicateWoodCost = {
		{EOceanResourceType::Wood, 5},
		{EOceanResourceType::Wood, 5}
	};
	TestFalse(TEXT("Duplicate wood cost aggregates for affordability"), Inventory->CanAfford(DuplicateWoodCost));
	TestFalse(TEXT("Duplicate wood cost spend is atomic"), Inventory->TrySpend(DuplicateWoodCost));
	TestEqual(TEXT("Wood unchanged after duplicate failed spend"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 6);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPSurvivalTickTest, "Ocean.MVP.Survival.Tick", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPSurvivalTickTest::RunTest(const FString& Parameters)
{
	UOceanSurvivalComponent* Survival = NewObject<UOceanSurvivalComponent>();
	Survival->SetStats(50.0f, 80.0f, 70.0f);
	Survival->ApplySurvivalDelta(1.0f);
	TestTrue(TEXT("Default stamina recovery applies"), FMath::IsNearlyEqual(Survival->GetStamina(), 52.0f));
	TestTrue(TEXT("Default hydration drain applies"), FMath::IsNearlyEqual(Survival->GetHydration(), 79.6f));
	TestTrue(TEXT("Default satiety drain applies"), FMath::IsNearlyEqual(Survival->GetSatiety(), 69.75f));

	Survival->SetHydrationDrainPerSecond(2.0f);
	Survival->SetSatietyDrainPerSecond(3.0f);
	Survival->SetStaminaRecoverPerSecond(5.0f);
	Survival->SetStats(50.0f, 80.0f, 70.0f);

	Survival->ApplySurvivalDelta(10.0f);
	TestEqual(TEXT("Stamina recovers and clamps"), Survival->GetStamina(), 100.0f);
	TestEqual(TEXT("Hydration drains"), Survival->GetHydration(), 60.0f);
	TestEqual(TEXT("Satiety drains"), Survival->GetSatiety(), 40.0f);

	Survival->SetStaminaRecoverPerSecond(-1.0f);
	Survival->SetHydrationDrainPerSecond(-1.0f);
	Survival->SetSatietyDrainPerSecond(-1.0f);
	Survival->SetStats(50.0f, 80.0f, 70.0f);
	Survival->ApplySurvivalDelta(10.0f);
	TestEqual(TEXT("Negative stamina recovery clamps to zero"), Survival->GetStamina(), 50.0f);
	TestEqual(TEXT("Negative hydration drain clamps to zero"), Survival->GetHydration(), 80.0f);
	TestEqual(TEXT("Negative satiety drain clamps to zero"), Survival->GetSatiety(), 70.0f);

	return true;
}

#endif
