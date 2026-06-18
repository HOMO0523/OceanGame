#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/UI/OceanBackpackSlotWidget.h"
#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/UI/OceanBuildPanelWidget.h"
#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "OceanPrototype/UI/OceanItemUseModalWidget.h"
#include "OceanPrototype/UI/OceanPauseMenuWidget.h"
#include "OceanPrototype/UI/OceanSaveSlotWidget.h"
#include "OceanPrototype/UI/OceanSettingsWidget.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Slider.h"
#include "Components/Widget.h"

namespace
{
template <typename WidgetType>
WidgetType* FindWidgetByName(UUserWidget* RootWidget, const TCHAR* WidgetName)
{
	if (!RootWidget || !RootWidget->WidgetTree)
	{
		return nullptr;
	}

	TArray<UWidget*> AllWidgets;
	RootWidget->WidgetTree->GetAllWidgets(AllWidgets);
	for (UWidget* Widget : AllWidgets)
	{
		if (Widget && Widget->GetFName() == FName(WidgetName))
		{
			return Cast<WidgetType>(Widget);
		}
	}
	return nullptr;
}
}

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

// --- Menu Buttons ---

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanSettingsWidgetButtonsAndLayoutTest, "Ocean.UI.Settings.ButtonsAndLayout", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanSettingsWidgetButtonsAndLayoutTest::RunTest(const FString& Parameters)
{
	auto* Settings = NewObject<UOceanSettingsWidget>(GetTransientPackage(), UOceanSettingsWidget::StaticClass());
	TestNotNull(TEXT("[TDD] OceanSettings_WidgetCreated"), Settings);
	Settings->Initialize();
	Settings->TakeWidget();

	auto* BGMSlider = FindWidgetByName<USlider>(Settings, TEXT("BGMSlider"));
	auto* SFXSlider = FindWidgetByName<USlider>(Settings, TEXT("SFXSlider"));
	auto* SaveButton = FindWidgetByName<UButton>(Settings, TEXT("SaveButton"));
	auto* CloseButton = FindWidgetByName<UButton>(Settings, TEXT("CloseButton"));
	auto* Box = FindWidgetByName<UWidget>(Settings, TEXT("Box"));

	TestNotNull(TEXT("[TDD] OceanSettings_BGMSlider"), BGMSlider);
	TestNotNull(TEXT("[TDD] OceanSettings_SFXSlider"), SFXSlider);
	TestNotNull(TEXT("[TDD] OceanSettings_SaveButton"), SaveButton);
	TestNotNull(TEXT("[TDD] OceanSettings_CloseButton"), CloseButton);
	TestNotNull(TEXT("[TDD] OceanSettings_Box"), Box);

	if (BGMSlider)
	{
		TestTrue(TEXT("[TDD] OceanSettings_BGMSliderBound"), BGMSlider->OnValueChanged.IsBound());
	}
	if (SFXSlider)
	{
		TestTrue(TEXT("[TDD] OceanSettings_SFXSliderBound"), SFXSlider->OnValueChanged.IsBound());
	}
	if (SaveButton)
	{
		TestTrue(TEXT("[TDD] OceanSettings_SaveButtonBound"), SaveButton->OnClicked.IsBound());
	}
	if (CloseButton)
	{
		TestTrue(TEXT("[TDD] OceanSettings_CloseButtonBound"), CloseButton->OnClicked.IsBound());
	}
	if (Box)
	{
		auto* CanvasSlot = Cast<UCanvasPanelSlot>(Box->Slot);
		TestNotNull(TEXT("[TDD] OceanSettings_BoxCanvasSlot"), CanvasSlot);
		if (CanvasSlot)
		{
			TestEqual(TEXT("[TDD] OceanSettings_BoxAnchorMin"), CanvasSlot->GetAnchors().Minimum, FVector2D(0.5f, 0.5f));
			TestEqual(TEXT("[TDD] OceanSettings_BoxAnchorMax"), CanvasSlot->GetAnchors().Maximum, FVector2D(0.5f, 0.5f));
			TestEqual(TEXT("[TDD] OceanSettings_BoxAlignment"), CanvasSlot->GetAlignment(), FVector2D(0.5f, 0.5f));
			TestEqual(TEXT("[TDD] OceanSettings_BoxPosition"), CanvasSlot->GetPosition(), FVector2D(0.0f, 0.0f));
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanPauseMenuButtonsTest, "Ocean.UI.PauseMenu.ButtonsBound", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanPauseMenuButtonsTest::RunTest(const FString& Parameters)
{
	auto* PauseMenu = NewObject<UOceanPauseMenuWidget>(GetTransientPackage(), UOceanPauseMenuWidget::StaticClass());
	TestNotNull(TEXT("[TDD] OceanPauseMenu_WidgetCreated"), PauseMenu);
	PauseMenu->Initialize();
	PauseMenu->TakeWidget();

	auto* ResumeButton = FindWidgetByName<UButton>(PauseMenu, TEXT("ResumeButton"));
	auto* SettingsButton = FindWidgetByName<UButton>(PauseMenu, TEXT("SettingsButton"));
	auto* QuitButton = FindWidgetByName<UButton>(PauseMenu, TEXT("QuitToMenuButton"));

	TestNotNull(TEXT("[TDD] OceanPauseMenu_ResumeButton"), ResumeButton);
	TestNotNull(TEXT("[TDD] OceanPauseMenu_SettingsButton"), SettingsButton);
	TestNotNull(TEXT("[TDD] OceanPauseMenu_QuitButton"), QuitButton);

	if (ResumeButton)
	{
		TestTrue(TEXT("[TDD] OceanPauseMenu_ResumeBound"), ResumeButton->OnClicked.IsBound());
	}
	if (SettingsButton)
	{
		TestTrue(TEXT("[TDD] OceanPauseMenu_SettingsBound"), SettingsButton->OnClicked.IsBound());
	}
	if (QuitButton)
	{
		TestTrue(TEXT("[TDD] OceanPauseMenu_QuitBound"), QuitButton->OnClicked.IsBound());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanSaveSlotButtonsTest, "Ocean.UI.SaveSlot.ButtonsBound", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanSaveSlotButtonsTest::RunTest(const FString& Parameters)
{
	auto* Slot = NewObject<UOceanSaveSlotWidget>(GetTransientPackage(), UOceanSaveSlotWidget::StaticClass());
	TestNotNull(TEXT("[TDD] OceanSaveSlot_WidgetCreated"), Slot);
	Slot->Initialize();
	Slot->TakeWidget();

	auto* LoadButton = FindWidgetByName<UButton>(Slot, TEXT("LoadButton"));
	auto* DeleteButton = FindWidgetByName<UButton>(Slot, TEXT("DeleteButton"));

	TestNotNull(TEXT("[TDD] OceanSaveSlot_LoadButton"), LoadButton);
	TestNotNull(TEXT("[TDD] OceanSaveSlot_DeleteButton"), DeleteButton);
	if (LoadButton)
	{
		TestTrue(TEXT("[TDD] OceanSaveSlot_LoadBound"), LoadButton->OnClicked.IsBound());
	}
	if (DeleteButton)
	{
		TestTrue(TEXT("[TDD] OceanSaveSlot_DeleteBound"), DeleteButton->OnClicked.IsBound());
	}

	return true;
}

#endif
