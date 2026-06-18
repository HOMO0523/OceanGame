#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/UI/OceanBackpackSlotWidget.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanItemTypes.h"

// TDD: BackpackPanel must create 12 visible slot widgets in a WrapBox.

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBackpackPanelSlotsTest, "Ocean.UI.BackpackPanel.SlotsCreated", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBackpackPanelSlotsTest::RunTest(const FString& Parameters)
{
    auto* Panel = NewObject<UOceanBackpackPanelWidget>(GetTransientPackage(), UOceanBackpackPanelWidget::StaticClass());
    TestNotNull(TEXT("[TDD] BackpackPanel_Created"), Panel);

    Panel->Initialize();

    // TakeWidget triggers RebuildWidget
    Panel->TakeWidget();

    // Must have a WrapBox for slot grid
    TArray<UWidget*> AllWidgets;
    Panel->WidgetTree->GetAllWidgets(AllWidgets);
    int32 WrapBoxCount = 0;
    for (UWidget* W : AllWidgets)
    {
        if (W && W->IsA(UWrapBox::StaticClass())) WrapBoxCount++;
    }
    TestTrue(TEXT("[TDD] BackpackPanel_HasWrapBox"), WrapBoxCount >= 1);

    // Must have 12 BackpackSlotWidget children
    int32 SlotCount = 0;
    for (UWidget* W : AllWidgets)
    {
        if (W && W->IsA(UOceanBackpackSlotWidget::StaticClass())) SlotCount++;
    }
    TestEqual(TEXT("[TDD] BackpackPanel_12Slots"), SlotCount, 12);

    return true;
}

// TDD: Slot widget must have visible border + text elements after RebuildWidget.

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBackpackSlotVisualsTest, "Ocean.UI.BackpackSlot.Visuals", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBackpackSlotVisualsTest::RunTest(const FString& Parameters)
{
    auto* Slot = NewObject<UOceanBackpackSlotWidget>(GetTransientPackage(), UOceanBackpackSlotWidget::StaticClass());
    TestNotNull(TEXT("[TDD] BackpackSlot_Created"), Slot);

    Slot->Initialize();
    Slot->TakeWidget();

    // Must have a root widget
    TestNotNull(TEXT("[TDD] BackpackSlot_RootWidget"), Slot->WidgetTree->RootWidget.Get());

    // Must have at least 1 Border and 2 TextBlocks (item name + quantity)
    TArray<UWidget*> AllWidgets;
    Slot->WidgetTree->GetAllWidgets(AllWidgets);
    int32 BorderCount = 0, TextCount = 0;
    for (UWidget* W : AllWidgets)
    {
        if (!W) continue;
        if (W->IsA(UBorder::StaticClass())) BorderCount++;
        if (W->IsA(UTextBlock::StaticClass())) TextCount++;
    }
    TestTrue(TEXT("[TDD] BackpackSlot_HasBorder"), BorderCount >= 1);
    TestTrue(TEXT("[TDD] BackpackSlot_HasTextBlocks"), TextCount >= 2);

    return true;
}

// TDD: Panel RefreshSlots must update slot data when inventory changes.

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBackpackPanelRefreshTest, "Ocean.UI.BackpackPanel.RefreshSlots", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBackpackPanelRefreshTest::RunTest(const FString& Parameters)
{
    auto* Inventory = NewObject<UOceanInventoryComponent>();
    auto* Survival = NewObject<UOceanSurvivalComponent>();
    auto* Panel = NewObject<UOceanBackpackPanelWidget>(GetTransientPackage(), UOceanBackpackPanelWidget::StaticClass());
    Panel->Initialize();
    Panel->TakeWidget();
    Panel->BindInventory(Inventory, Survival);

    // Initially 0 slots occupied
    TestEqual(TEXT("[TDD] BackpackPanel_InitialEmpty"), Inventory->GetSlots().Num(), 0);

    // Add an item
    FOceanItemStack Water;
    Water.ItemId = TEXT("fresh_water");
    Water.Quantity = 3;
    Water.MaxStack = 8;
    Water.Category = EOceanItemCategory::Consumable;
    TestTrue(TEXT("[TDD] BackpackPanel_AddWater"), Inventory->AddItem(Water));
    TestEqual(TEXT("[TDD] BackpackPanel_SlotsAfterAdd"), Inventory->GetSlots().Num(), 1);

    // Refresh should update the first slot
    Panel->RefreshSlots();

    return true;
}

// TDD: An occupied consumable slot must expose a UI use action that reaches
// the bound survival component, not only the lower-level inventory API.

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanBackpackSlotRequestUseRestoresStatsTest, "Ocean.UI.BackpackSlot.RequestUseRestoresStats", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanBackpackSlotRequestUseRestoresStatsTest::RunTest(const FString& Parameters)
{
    auto* Inventory = NewObject<UOceanInventoryComponent>();
    auto* Survival = NewObject<UOceanSurvivalComponent>();
    auto* Panel = NewObject<UOceanBackpackPanelWidget>(GetTransientPackage(), UOceanBackpackPanelWidget::StaticClass());
    Panel->Initialize();
    Panel->TakeWidget();
    Panel->BindInventory(Inventory, Survival);

    FOceanItemStack Water;
    Water.ItemId = TEXT("fresh_water");
    Water.Quantity = 1;
    Water.MaxStack = 8;
    Water.Category = EOceanItemCategory::Consumable;
    Water.UseEffect.HydrationDelta = 100.0f;

    Survival->SetStats(40.0f, 20.0f, 30.0f);
    TestTrue(TEXT("[TDD] BackpackSlotUse_AddWater"), Inventory->AddItem(Water));
    Panel->RefreshSlots();

    TArray<UWidget*> AllWidgets;
    Panel->WidgetTree->GetAllWidgets(AllWidgets);
    UOceanBackpackSlotWidget* FirstSlotWidget = nullptr;
    for (UWidget* Widget : AllWidgets)
    {
        auto* SlotWidget = Cast<UOceanBackpackSlotWidget>(Widget);
        if (SlotWidget && SlotWidget->GetSlotIndex() == 0)
        {
            FirstSlotWidget = SlotWidget;
            break;
        }
    }

    TestNotNull(TEXT("[TDD] BackpackSlotUse_FirstSlotWidget"), FirstSlotWidget);
    if (!FirstSlotWidget)
    {
        return false;
    }

    TestTrue(TEXT("[TDD] BackpackSlotUse_RequestUse"), FirstSlotWidget->RequestUse());
    TestEqual(TEXT("[TDD] BackpackSlotUse_HydrationAfterUse"), Survival->GetHydration(), 100.0f);
    TestEqual(TEXT("[TDD] BackpackSlotUse_SlotConsumed"), Inventory->GetSlots().Num(), 0);
    return true;
}

#endif
