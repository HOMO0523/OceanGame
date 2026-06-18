#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanPrototype/OceanSurvivalComponent.h"

namespace
{
FOceanItemStack MakeTestItemStack(const TCHAR* ItemId, int32 Quantity, int32 MaxStack)
{
	FOceanItemStack Stack;
	Stack.ItemId = ItemId;
	Stack.Quantity = Quantity;
	Stack.MaxStack = MaxStack;
	return Stack;
}
}

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryRejectsInvalidItemsTest, "Ocean.MVP.Inventory.RejectsInvalidItems", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryRejectsInvalidItemsTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	TestNotNull(TEXT("[TDD] OceanInventory_InvalidComponent"), Inventory);

	FOceanItemStack MissingItemId;
	MissingItemId.Quantity = 1;
	MissingItemId.MaxStack = 8;
	TestFalse(TEXT("[TDD] OceanInventory_RejectsMissingItemId"), Inventory->AddItem(MissingItemId));
	TestFalse(TEXT("[TDD] OceanInventory_RejectsZeroQuantity"), Inventory->AddItem(MakeTestItemStack(TEXT("canned_food"), 0, 8)));
	TestFalse(TEXT("[TDD] OceanInventory_RejectsZeroMaxStack"), Inventory->AddItem(MakeTestItemStack(TEXT("canned_food"), 1, 0)));
	TestEqual(TEXT("[TDD] OceanInventory_InvalidDoesNotCreateSlots"), Inventory->GetSlots().Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryMergeAndSplitTest, "Ocean.MVP.Inventory.MergeAndSplit", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryMergeAndSplitTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	TestNotNull(TEXT("[TDD] OceanInventory_MergeSplitComponent"), Inventory);

	TestTrue(TEXT("[TDD] OceanInventory_AddInitialMergeStack"), Inventory->AddItem(MakeTestItemStack(TEXT("canned_food"), 2, 8)));
	TestTrue(TEXT("[TDD] OceanInventory_MergeIntoExistingStack"), Inventory->AddItem(MakeTestItemStack(TEXT("canned_food"), 3, 8)));
	TestEqual(TEXT("[TDD] OceanInventory_MergedSlotCount"), Inventory->GetSlots().Num(), 1);
	TestEqual(TEXT("[TDD] OceanInventory_MergedQuantity"), Inventory->GetSlots()[0].Stack.Quantity, 5);

	TestTrue(TEXT("[TDD] OceanInventory_SplitOverflowStack"), Inventory->AddItem(MakeTestItemStack(TEXT("rope"), 10, 4)));
	TestEqual(TEXT("[TDD] OceanInventory_SplitSlotCount"), Inventory->GetSlots().Num(), 4);
	TestEqual(TEXT("[TDD] OceanInventory_SplitFirstQuantity"), Inventory->GetSlots()[1].Stack.Quantity, 4);
	TestEqual(TEXT("[TDD] OceanInventory_SplitSecondQuantity"), Inventory->GetSlots()[2].Stack.Quantity, 4);
	TestEqual(TEXT("[TDD] OceanInventory_SplitThirdQuantity"), Inventory->GetSlots()[3].Stack.Quantity, 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryFailedCapacityAtomicTest, "Ocean.MVP.Inventory.FailedCapacityAtomic", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryFailedCapacityAtomicTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	TestNotNull(TEXT("[TDD] OceanInventory_AtomicComponent"), Inventory);

	TestTrue(TEXT("[TDD] OceanInventory_AddPartialExistingStack"), Inventory->AddItem(MakeTestItemStack(TEXT("apple"), 7, 8)));
	for (int32 Index = 0; Index < 11; ++Index)
	{
		const FString ItemName = FString::Printf(TEXT("filler_%d"), Index);
		TestTrue(TEXT("[TDD] OceanInventory_FillCapacity"), Inventory->AddItem(MakeTestItemStack(*ItemName, 1, 1)));
	}

	TestEqual(TEXT("[TDD] OceanInventory_AtomicPreSlotCount"), Inventory->GetSlots().Num(), 12);
	TestEqual(TEXT("[TDD] OceanInventory_AtomicPreQuantity"), Inventory->GetSlots()[0].Stack.Quantity, 7);
	TestFalse(TEXT("[TDD] OceanInventory_RejectsOverflowAtomically"), Inventory->AddItem(MakeTestItemStack(TEXT("apple"), 2, 8)));
	TestEqual(TEXT("[TDD] OceanInventory_AtomicPostSlotCount"), Inventory->GetSlots().Num(), 12);
	TestEqual(TEXT("[TDD] OceanInventory_AtomicPostQuantity"), Inventory->GetSlots()[0].Stack.Quantity, 7);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryUseItemTest, "Ocean.MVP.Inventory.UseItem", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryUseItemTest::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>();
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>(Owner);
	UOceanSurvivalComponent* Survival = NewObject<UOceanSurvivalComponent>(Owner);

	FOceanItemStack Water;
	Water.ItemId = TEXT("fresh_water");
	Water.Quantity = 1;
	Water.MaxStack = 4;
	Water.Category = EOceanItemCategory::Consumable;
	Water.UseEffect.HydrationDelta = 100.0f;

	Survival->SetStats(40.0f, 20.0f, 30.0f);
	TestTrue(TEXT("[TDD] OceanInventory_AddWaterItem"), Inventory->AddItem(Water));
	TestTrue(TEXT("[TDD] OceanInventory_UseWaterItem"), Inventory->TryUseItemAtSlot(0, Survival));
	TestEqual(TEXT("[TDD] OceanInventory_HydrationAfterWater"), Survival->GetHydration(), 100.0f);
	TestEqual(TEXT("[TDD] OceanInventory_WaterSlotConsumed"), Inventory->GetSlots().Num(), 0);
	TestFalse(TEXT("[TDD] OceanInventory_UseMissingItemFails"), Inventory->TryUseItemAtSlot(0, Survival));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryDragDropTest, "Ocean.MVP.Inventory.DragDropModel", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryDragDropTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();

	FOceanItemStack Wood;
	Wood.ItemId = TEXT("wood");
	Wood.Quantity = 3;
	Wood.MaxStack = 8;
	Wood.Category = EOceanItemCategory::Resource;

	FOceanItemStack Scrap;
	Scrap.ItemId = TEXT("scrap");
	Scrap.Quantity = 2;
	Scrap.MaxStack = 8;
	Scrap.Category = EOceanItemCategory::Resource;

	TestTrue(TEXT("[TDD] OceanInventory_AddWoodForDrag"), Inventory->AddItem(Wood));
	TestTrue(TEXT("[TDD] OceanInventory_AddScrapForDrag"), Inventory->AddItem(Scrap));
	TestEqual(TEXT("[TDD] OceanInventory_DragSwap"), Inventory->MoveOrMergeSlot(0, 1), EOceanInventoryDragDropResult::Swapped);
	TestEqual(TEXT("[TDD] OceanInventory_SlotZeroAfterSwap"), Inventory->GetSlots()[0].Stack.ItemId, FName(TEXT("scrap")));
	TestEqual(TEXT("[TDD] OceanInventory_InvalidDragReject"), Inventory->MoveOrMergeSlot(0, 99), EOceanInventoryDragDropResult::Rejected);
	TestEqual(TEXT("[TDD] OceanInventory_SlotCountAfterInvalidDrag"), Inventory->GetSlots().Num(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryDragDropMergeTest, "Ocean.MVP.Inventory.DragDropMerge", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryDragDropMergeTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();

	TestTrue(TEXT("[TDD] OceanInventory_AddPartialMergeStack"), Inventory->AddItem(MakeTestItemStack(TEXT("wood"), 8, 8)));
	TestTrue(TEXT("[TDD] OceanInventory_AddOverflowMergeStack"), Inventory->AddItem(MakeTestItemStack(TEXT("wood"), 3, 8)));
	TestEqual(TEXT("[TDD] OceanInventory_PreMergeSlotCount"), Inventory->GetSlots().Num(), 2);
	TArray<FOceanInventorySlot>& MutableSlots = const_cast<TArray<FOceanInventorySlot>&>(Inventory->GetSlots());
	MutableSlots[0].Stack.Quantity = 5;
	TestEqual(TEXT("[TDD] OceanInventory_DragMerge"), Inventory->MoveOrMergeSlot(1, 0), EOceanInventoryDragDropResult::Merged);
	TestEqual(TEXT("[TDD] OceanInventory_MergedTargetQuantity"), Inventory->GetSlots()[0].Stack.Quantity, 8);
	TestEqual(TEXT("[TDD] OceanInventory_MergedSourceRemoved"), Inventory->GetSlots().Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryDragDropRejectsLockedTest, "Ocean.MVP.Inventory.DragDropRejectsLocked", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryDragDropRejectsLockedTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();

	TestTrue(TEXT("[TDD] OceanInventory_AddLockedWood"), Inventory->AddItem(MakeTestItemStack(TEXT("wood"), 1, 8)));
	TestTrue(TEXT("[TDD] OceanInventory_AddLockedScrap"), Inventory->AddItem(MakeTestItemStack(TEXT("scrap"), 1, 8)));

	TArray<FOceanInventorySlot>& MutableSlots = const_cast<TArray<FOceanInventorySlot>&>(Inventory->GetSlots());
	MutableSlots[1].bLocked = true;

	TestEqual(TEXT("[TDD] OceanInventory_LockedDragReject"), Inventory->MoveOrMergeSlot(0, 1), EOceanInventoryDragDropResult::Rejected);
	TestEqual(TEXT("[TDD] OceanInventory_LockedDragSlotZero"), Inventory->GetSlots()[0].Stack.ItemId, FName(TEXT("wood")));
	TestEqual(TEXT("[TDD] OceanInventory_LockedDragSlotOne"), Inventory->GetSlots()[1].Stack.ItemId, FName(TEXT("scrap")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryRestoreStateTest, "Ocean.MVP.Inventory.RestoreState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryRestoreStateTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	TestNotNull(TEXT("[TDD] OceanInventory_RestoreComponent"), Inventory);

	TArray<FOceanResourceStack> SavedResources;
	SavedResources.Add({ EOceanResourceType::Wood, 7 });
	SavedResources.Add({ EOceanResourceType::Food, 2 });

	TArray<FOceanInventorySlot> SavedSlots;
	FOceanInventorySlot DiveSuitSlot;
	DiveSuitSlot.SlotIndex = 9;
	DiveSuitSlot.Stack.ItemId = FName(TEXT("dive_suit"));
	DiveSuitSlot.Stack.Quantity = 1;
	DiveSuitSlot.Stack.MaxStack = 1;
	DiveSuitSlot.Stack.Category = EOceanItemCategory::KeyItem;
	DiveSuitSlot.Stack.bKeyItem = true;
	SavedSlots.Add(DiveSuitSlot);

	FOceanInventorySlot WaterSlot;
	WaterSlot.SlotIndex = 3;
	WaterSlot.Stack.ItemId = FName(TEXT("fresh_water"));
	WaterSlot.Stack.Quantity = 2;
	WaterSlot.Stack.MaxStack = 4;
	WaterSlot.Stack.Category = EOceanItemCategory::Consumable;
	WaterSlot.Stack.UseEffect.HydrationDelta = 35.0f;
	SavedSlots.Add(WaterSlot);

	TestTrue(TEXT("[TDD] OceanInventory_RestoreStateAccepted"), Inventory->RestoreInventoryState(SavedResources, SavedSlots));
	TestEqual(TEXT("[TDD] OceanInventory_RestoreWood"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 7);
	TestEqual(TEXT("[TDD] OceanInventory_RestoreFood"), Inventory->GetResourceAmount(EOceanResourceType::Food), 2);
	TestEqual(TEXT("[TDD] OceanInventory_RestoreSlotCount"), Inventory->GetSlots().Num(), 2);
	TestEqual(TEXT("[TDD] OceanInventory_RestoreReindexedFirst"), Inventory->GetSlots()[0].SlotIndex, 0);
	TestEqual(TEXT("[TDD] OceanInventory_RestoreReindexedSecond"), Inventory->GetSlots()[1].SlotIndex, 1);
	TestTrue(TEXT("[TDD] OceanInventory_RestoreDiveSuit"), Inventory->HasItem(FName(TEXT("dive_suit"))));
	TestTrue(TEXT("[TDD] OceanInventory_RestoreFreshWater"), Inventory->HasItem(FName(TEXT("fresh_water"))));
	return true;
}

#endif
