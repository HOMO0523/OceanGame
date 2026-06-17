#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanItemTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventorySlotModelTest, "Ocean.MVP.Inventory.SlotModel", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventorySlotModelTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	TestNotNull(TEXT("[TDD] OceanInventory_SlotModelComponent"), Inventory);

	FOceanItemStack CannedFood;
	CannedFood.ItemId = TEXT("canned_food");
	CannedFood.Quantity = 2;
	CannedFood.MaxStack = 8;
	CannedFood.Category = EOceanItemCategory::Consumable;
	CannedFood.UseEffect.SatietyDelta = 100.0f;

	TestTrue(TEXT("[TDD] OceanInventory_AddItemSlot"), Inventory->AddItem(CannedFood));
	TestEqual(TEXT("[TDD] OceanInventory_SlotCount"), Inventory->GetSlots().Num(), 1);
	TestEqual(TEXT("[TDD] OceanInventory_FirstSlotItem"), Inventory->GetSlots()[0].Stack.ItemId, FName(TEXT("canned_food")));
	TestEqual(TEXT("[TDD] OceanInventory_FirstSlotQuantity"), Inventory->GetSlots()[0].Stack.Quantity, 2);
	return true;
}

#endif
