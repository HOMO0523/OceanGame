#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"

// TDD: StatusPanel must create 3 visual stamina cells, 3 progress bars,
// and be idempotent (RebuildWidget called twice must not duplicate).

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanStatusPanelVisualsTest, "Ocean.UI.StatusPanel.Visuals", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanStatusPanelVisualsTest::RunTest(const FString& Parameters)
{
    auto* W = NewObject<UOceanStatusPanelWidget>(GetTransientPackage(), UOceanStatusPanelWidget::StaticClass());
    TestNotNull(TEXT("[TDD] StatusPanel_Created"), W);

    // TakeWidget triggers RebuildWidget
    W->TakeWidget();

    // Must have a WidgetTree with a root
    TestNotNull(TEXT("[TDD] StatusPanel_RootWidget"), W->WidgetTree->RootWidget.Get());

    // Count HorizontalBox children (stamina cells container)
    auto FindWidgetsByClass = [](UWidgetTree* Tree, UClass* Class) -> TArray<UWidget*>
    {
        TArray<UWidget*> Result;
        TArray<UWidget*> All;
        Tree->GetAllWidgets(All);
        for (UWidget* Widget : All)
        {
            if (Widget && Widget->IsA(Class))
            {
                Result.Add(Widget);
            }
        }
        return Result;
    };

    // Should have at least 3 ProgressBar widgets (hydration, satiety, health)
    auto Bars = FindWidgetsByClass(W->WidgetTree, UProgressBar::StaticClass());
    TestTrue(TEXT("[TDD] StatusPanel_Has3ProgressBars"), Bars.Num() >= 3);

    // Should have at least 3 Border widgets for stamina cells
    auto Borders = FindWidgetsByClass(W->WidgetTree, UBorder::StaticClass());
    TestTrue(TEXT("[TDD] StatusPanel_Has3CellBorders"), Borders.Num() >= 3);

    // Should have at least 1 HorizontalBox for the cell row
    auto HBoxes = FindWidgetsByClass(W->WidgetTree, UHorizontalBox::StaticClass());
    TestTrue(TEXT("[TDD] StatusPanel_HasHorizontalBox"), HBoxes.Num() >= 1);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanStatusPanelIdempotentTest, "Ocean.UI.StatusPanel.Idempotent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanStatusPanelIdempotentTest::RunTest(const FString& Parameters)
{
    auto* W = NewObject<UOceanStatusPanelWidget>(GetTransientPackage(), UOceanStatusPanelWidget::StaticClass());

    // Call TakeWidget twice - should not crash or duplicate
    W->TakeWidget();
    W->TakeWidget();

    // Count progress bars - should still be 3, not 6
    TArray<UWidget*> All;
    W->WidgetTree->GetAllWidgets(All);
    int32 BarCount = 0;
    for (UWidget* Widget : All)
    {
        if (Widget && Widget->IsA(UProgressBar::StaticClass())) BarCount++;
    }
    TestEqual(TEXT("[TDD] StatusPanel_NoDuplicateBars"), BarCount, 3);

    // Count cell borders - should still be 3, not 6
    int32 BorderCount = 0;
    for (UWidget* Widget : All)
    {
        if (Widget && Widget->IsA(UBorder::StaticClass())) BorderCount++;
    }
    TestEqual(TEXT("[TDD] StatusPanel_NoDuplicateCells"), BorderCount, 3);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanStatusPanelCellFillTest, "Ocean.UI.StatusPanel.CellFill", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanStatusPanelCellFillTest::RunTest(const FString& Parameters)
{
    auto* Survival = NewObject<UOceanSurvivalComponent>();
    auto* W = NewObject<UOceanStatusPanelWidget>(GetTransientPackage(), UOceanStatusPanelWidget::StaticClass());
    W->TakeWidget();

    // Full stamina = 3 cells
    Survival->SetStats(100.0f, 100.0f, 100.0f);
    W->BindSurvivalComponent(Survival);
    TestEqual(TEXT("[TDD] StatusPanel_FullStamina_3Cells"), W->GetStaminaCells(), 3);

    // Half stamina = 2 cells (ceil(50/33.33) = 2)
    Survival->SetStats(50.0f, 100.0f, 100.0f);
    TestEqual(TEXT("[TDD] StatusPanel_HalfStamina_2Cells"), W->GetStaminaCells(), 2);

    // Low stamina = 1 cell
    Survival->SetStats(10.0f, 100.0f, 100.0f);
    TestEqual(TEXT("[TDD] StatusPanel_LowStamina_1Cell"), W->GetStaminaCells(), 1);

    // Zero stamina = 0 cells
    Survival->SetStats(0.0f, 100.0f, 100.0f);
    TestEqual(TEXT("[TDD] StatusPanel_ZeroStamina_0Cells"), W->GetStaminaCells(), 0);

    return true;
}

#endif
