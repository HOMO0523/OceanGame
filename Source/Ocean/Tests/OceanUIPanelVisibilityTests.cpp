#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Blueprint/WidgetTree.h"
#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "OceanPrototype/UI/OceanToastWidget.h"
#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/UI/OceanBuildPanelWidget.h"
#include "OceanPrototype/UI/OceanItemUseModalWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"

// TDD: 验证子面板 TakeWidget 后有可见的 SWidget
// Bug: 子面板在 NativeConstruct 中设置 WidgetTree->RootWidget，但 RebuildWidget 在 NativeConstruct 之前调用，
//      导致 RebuildWidget 返回空的 SBox，UI 不可见。
// Fix: 改为在 RebuildWidget override 中设置 RootWidget。

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanUIPanelVisibilityTest, "Ocean.UI.PanelVisibility.TakeWidget", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanUIPanelVisibilityTest::RunTest(const FString& Parameters)
{
    // TakeWidget() 是 public，内部调用 RebuildWidget()，返回 SWidget
    // 如果 RootWidget 没有在 RebuildWidget 中设置，TakeWidget 返回空的 SBox

    // 1. StatusPanel
    {
        auto* W = NewObject<UOceanStatusPanelWidget>(GetTransientPackage(), UOceanStatusPanelWidget::StaticClass());
        TestNotNull(TEXT("[TDD] OceanUIPanel_StatusPanel_Created"), W);
        TSharedRef<SWidget> Sw = W->TakeWidget();
        TestTrue(TEXT("[TDD] OceanUIPanel_StatusPanel_HasContent"), Sw != SNullWidget::NullWidget);
        TestNotNull(TEXT("[TDD] OceanUIPanel_StatusPanel_RootSet"), W->WidgetTree->RootWidget.Get());
    }

    // 2. TimePanel
    {
        auto* W = NewObject<UOceanTimePanelWidget>(GetTransientPackage(), UOceanTimePanelWidget::StaticClass());
        TSharedRef<SWidget> Sw = W->TakeWidget();
        TestTrue(TEXT("[TDD] OceanUIPanel_Time_HasContent"), Sw != SNullWidget::NullWidget);
        TestNotNull(TEXT("[TDD] OceanUIPanel_Time_RootSet"), W->WidgetTree->RootWidget.Get());
    }

    // 3. Toast
    {
        auto* W = NewObject<UOceanToastWidget>(GetTransientPackage(), UOceanToastWidget::StaticClass());
        TSharedRef<SWidget> Sw = W->TakeWidget();
        TestTrue(TEXT("[TDD] OceanUIPanel_Toast_HasContent"), Sw != SNullWidget::NullWidget);
        TestNotNull(TEXT("[TDD] OceanUIPanel_Toast_RootSet"), W->WidgetTree->RootWidget.Get());
    }

    // 4. Backpack
    {
        auto* W = NewObject<UOceanBackpackPanelWidget>(GetTransientPackage(), UOceanBackpackPanelWidget::StaticClass());
        TSharedRef<SWidget> Sw = W->TakeWidget();
        TestTrue(TEXT("[TDD] OceanUIPanel_Backpack_HasContent"), Sw != SNullWidget::NullWidget);
        TestNotNull(TEXT("[TDD] OceanUIPanel_Backpack_RootSet"), W->WidgetTree->RootWidget.Get());
    }

    // 5. Build
    {
        auto* W = NewObject<UOceanBuildPanelWidget>(GetTransientPackage(), UOceanBuildPanelWidget::StaticClass());
        TSharedRef<SWidget> Sw = W->TakeWidget();
        TestTrue(TEXT("[TDD] OceanUIPanel_Build_HasContent"), Sw != SNullWidget::NullWidget);
        TestNotNull(TEXT("[TDD] OceanUIPanel_Build_RootSet"), W->WidgetTree->RootWidget.Get());
    }

    // 6. Modal
    {
        auto* W = NewObject<UOceanItemUseModalWidget>(GetTransientPackage(), UOceanItemUseModalWidget::StaticClass());
        TSharedRef<SWidget> Sw = W->TakeWidget();
        TestTrue(TEXT("[TDD] OceanUIPanel_Modal_HasContent"), Sw != SNullWidget::NullWidget);
        TestNotNull(TEXT("[TDD] OceanUIPanel_Modal_RootSet"), W->WidgetTree->RootWidget.Get());
    }

    return true;
}

#endif
