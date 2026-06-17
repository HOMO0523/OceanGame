#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/UI/OceanBackpackSlotWidget.h"
#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/UI/OceanBuildPanelWidget.h"
#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "OceanPrototype/UI/OceanItemUseModalWidget.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanBuildComponent.h"

// --- BackpackSlot ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBackpackSlotDataTest, "Ocean.UI.BackpackSlot.Data", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBackpackSlotDataTest::RunTest(const FString& Parameters)
{
	auto* Slot = NewObject<UOceanBackpackSlotWidget>(GetTransientPackage(), UOceanBackpackSlotWidget::StaticClass());
	TestNotNull(TEXT("[TDD] OceanBackpackSlot_WidgetCreated"), Slot);

	FOceanInventorySlot SlotData;
	SlotData.SlotIndex = 3;
	SlotData.Stack.ItemId = TEXT("canned_food");
	SlotData.Stack.Quantity = 5;
	SlotData.Stack.MaxStack = 8;
	SlotData.Stack.Category = EOceanItemCategory::Consumable;

	Slot->SetSlotData(3, SlotData);
	TestTrue(TEXT("[TDD] OceanBackpackSlot_Occupied"), Slot->IsOccupied());
	TestEqual(TEXT("[TDD] OceanBackpackSlot_Index"), Slot->GetSlotIndex(), 3);
	TestEqual(TEXT("[TDD] OceanBackpackSlot_ItemId"), Slot->GetCachedStack().ItemId, FName(TEXT("canned_food")));

	Slot->ClearSlot();
	TestFalse(TEXT("[TDD] OceanBackpackSlot_Cleared"), Slot->IsOccupied());
	TestEqual(TEXT("[TDD] OceanBackpackSlot_ClearedIndex"), Slot->GetSlotIndex(), INDEX_NONE);

	return true;
}

// --- BackpackPanel ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBackpackPanelBindTest, "Ocean.UI.BackpackPanel.Bind", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBackpackPanelBindTest::RunTest(const FString& Parameters)
{
	auto* Inventory = NewObject<UOceanInventoryComponent>();
	auto* Survival = NewObject<UOceanSurvivalComponent>();
	auto* Panel = NewObject<UOceanBackpackPanelWidget>(GetTransientPackage(), UOceanBackpackPanelWidget::StaticClass());

	Panel->BindInventory(Inventory, Survival);
	TestEqual(TEXT("[TDD] OceanBackpackPanel_EmptySlots"), Panel->GetSlots().Num(), 0);

	TestEqual(TEXT("[TDD] OceanBackpackPanel_DropRejectedEmpty"),
		static_cast<int32>(Panel->HandleSlotDrop(0, 1)),
		static_cast<int32>(EOceanInventoryDragDropResult::Rejected));

	return true;
}

// --- BuildPanel ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBuildPanelBindTest, "Ocean.UI.BuildPanel.Bind", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBuildPanelBindTest::RunTest(const FString& Parameters)
{
	auto* Build = NewObject<UOceanBuildComponent>();
	auto* Panel = NewObject<UOceanBuildPanelWidget>(GetTransientPackage(), UOceanBuildPanelWidget::StaticClass());

	Panel->BindBuildComponent(Build);
	TestFalse(TEXT("[TDD] OceanBuildPanel_NotActive"), Panel->IsBuildModeActive());
	TestEqual(TEXT("[TDD] OceanBuildPanel_Rotation"), Panel->GetRotationQuarterTurns(), 0);
	TestNull(TEXT("[TDD] OceanBuildPanel_NoSelectedModule"), Panel->GetSelectedModule());

	return true;
}

// --- TimePanel ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanTimePanelTest, "Ocean.UI.TimePanel.Basic", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanTimePanelTest::RunTest(const FString& Parameters)
{
	auto* Panel = NewObject<UOceanTimePanelWidget>(GetTransientPackage(), UOceanTimePanelWidget::StaticClass());
	TestNotNull(TEXT("[TDD] OceanTimePanel_Created"), Panel);
	TestEqual(TEXT("[TDD] OceanTimePanel_DefaultDay"), Panel->GetDay(), 1);

	Panel->SetDayAndTime(3, EOceanTimeOfDay::Afternoon);
	TestEqual(TEXT("[TDD] OceanTimePanel_Day3"), Panel->GetDay(), 3);

	Panel->SetDay(0);
	TestEqual(TEXT("[TDD] OceanTimePanel_DayClamp"), Panel->GetDay(), 1);

	return true;
}

// --- ItemUseModal ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanItemUseModalFlowTest, "Ocean.UI.ItemUseModal.Flow", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanItemUseModalFlowTest::RunTest(const FString& Parameters)
{
	auto* Modal = NewObject<UOceanItemUseModalWidget>(GetTransientPackage(), UOceanItemUseModalWidget::StaticClass());
	TestNotNull(TEXT("[TDD] OceanItemUseModal_Created"), Modal);

	FOceanItemStack Item;
	Item.ItemId = TEXT("fresh_water");
	Item.Quantity = 1;
	Item.Category = EOceanItemCategory::Consumable;

	Modal->ShowConfirmation(5, Item);
	TestEqual(TEXT("[TDD] OceanItemUseModal_PendingSlot"), Modal->GetPendingSlotIndex(), 5);
	TestEqual(TEXT("[TDD] OceanItemUseModal_PendingItem"), Modal->GetPendingItem().ItemId, FName(TEXT("fresh_water")));

	Modal->Confirm();
	TestEqual(TEXT("[TDD] OceanItemUseModal_AfterConfirm"), Modal->GetPendingSlotIndex(), INDEX_NONE);

	Modal->ShowConfirmation(2, Item);
	Modal->Reject();
	TestEqual(TEXT("[TDD] OceanItemUseModal_AfterReject"), Modal->GetPendingSlotIndex(), INDEX_NONE);

	return true;
}

#endif
