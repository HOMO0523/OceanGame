#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanStatusPanelBindTest, "Ocean.UI.StatusPanel.Bind", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanStatusPanelBindTest::RunTest(const FString& Parameters)
{
	UOceanSurvivalComponent* Survival = NewObject<UOceanSurvivalComponent>();
	Survival->SetStats(100.0f, 80.0f, 60.0f);

	UOceanStatusPanelWidget* StatusPanel = NewObject<UOceanStatusPanelWidget>(GetTransientPackage(), UOceanStatusPanelWidget::StaticClass());
	TestNotNull(TEXT("[TDD] OceanStatusPanel_WidgetCreated"), StatusPanel);

	StatusPanel->BindSurvivalComponent(Survival);

	TestTrue(TEXT("[TDD] OceanStatusPanel_StaminaPercent"),
		FMath::IsNearlyEqual(StatusPanel->GetStaminaPercent(), 1.0f));
	TestTrue(TEXT("[TDD] OceanStatusPanel_HydrationPercent"),
		FMath::IsNearlyEqual(StatusPanel->GetHydrationPercent(), 0.8f));
	TestTrue(TEXT("[TDD] OceanStatusPanel_SatietyPercent"),
		FMath::IsNearlyEqual(StatusPanel->GetSatietyPercent(), 0.6f));

	// 体力格数：100/33.33 = 3.0，向上取整 = 3
	TestEqual(TEXT("[TDD] OceanStatusPanel_StaminaCells_Full"), StatusPanel->GetStaminaCells(), 3);
	TestEqual(TEXT("[TDD] OceanStatusPanel_MaxCells"), StatusPanel->GetMaxStaminaCells(), 3);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanStatusPanelUpdateTest, "Ocean.UI.StatusPanel.Update", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanStatusPanelUpdateTest::RunTest(const FString& Parameters)
{
	UOceanSurvivalComponent* Survival = NewObject<UOceanSurvivalComponent>();
	Survival->SetStats(100.0f, 100.0f, 100.0f);

	UOceanStatusPanelWidget* StatusPanel = NewObject<UOceanStatusPanelWidget>(GetTransientPackage(), UOceanStatusPanelWidget::StaticClass());
	StatusPanel->BindSurvivalComponent(Survival);

	// 改变生存数值
	Survival->SetStats(50.0f, 30.0f, 80.0f);

	// 验证 Widget getter 同步反映新值
	TestTrue(TEXT("[TDD] OceanStatusPanel_UpdatedStamina"),
		FMath::IsNearlyEqual(StatusPanel->GetStaminaPercent(), 0.5f));
	TestTrue(TEXT("[TDD] OceanStatusPanel_UpdatedHydration"),
		FMath::IsNearlyEqual(StatusPanel->GetHydrationPercent(), 0.3f));
	TestTrue(TEXT("[TDD] OceanStatusPanel_UpdatedSatiety"),
		FMath::IsNearlyEqual(StatusPanel->GetSatietyPercent(), 0.8f));

	// 50/33.33 = 1.5，向上取整 = 2
	TestEqual(TEXT("[TDD] OceanStatusPanel_UpdatedCells"), StatusPanel->GetStaminaCells(), 2);

	// 测试边界：体力为零
	Survival->SetStats(0.0f, 0.0f, 0.0f);
	TestEqual(TEXT("[TDD] OceanStatusPanel_ZeroStaminaCells"), StatusPanel->GetStaminaCells(), 0);
	TestTrue(TEXT("[TDD] OceanStatusPanel_ZeroPercent"),
		FMath::IsNearlyEqual(StatusPanel->GetStaminaPercent(), 0.0f));

	// 测试解绑
	StatusPanel->UnbindSurvivalComponent();
	TestTrue(TEXT("[TDD] OceanStatusPanel_UnboundReturnsZero"),
		FMath::IsNearlyEqual(StatusPanel->GetStaminaPercent(), 0.0f));

	// 测试空绑定安全
	StatusPanel->BindSurvivalComponent(nullptr);
	TestTrue(TEXT("[TDD] OceanStatusPanel_NullBindSafe"),
		FMath::IsNearlyEqual(StatusPanel->GetHydrationPercent(), 0.0f));

	return true;
}

#endif
