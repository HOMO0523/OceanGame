#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/UI/OceanHUDRootWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPUIHUDStateModelTest, "Ocean.MVP.UI.HUDStateModel", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPUIHUDStateModelTest::RunTest(const FString& Parameters)
{
	UOceanHUDRootWidget* Widget = NewObject<UOceanHUDRootWidget>();
	TestNotNull(TEXT("[TDD] OceanUI_HUDRootWidget"), Widget);
	TestFalse(TEXT("[TDD] OceanUI_BackpackClosedByDefault"), Widget->IsBackpackOpen());
	Widget->SetBackpackOpen(true);
	TestTrue(TEXT("[TDD] OceanUI_BackpackOpenAfterSet"), Widget->IsBackpackOpen());
	Widget->ToggleBackpack();
	TestFalse(TEXT("[TDD] OceanUI_BackpackClosedAfterToggle"), Widget->IsBackpackOpen());
	return true;
}

#endif
