#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanGameMode.h"
#include "OceanPrototype/OceanSurvivalHUD.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPHUDGameModeUsesSurvivalHUDTest, "Ocean.MVP.HUD.GameModeUsesSurvivalHUD", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPHUDGameModeUsesSurvivalHUDTest::RunTest(const FString& Parameters)
{
	const AOceanGameMode* GameModeCDO = GetDefault<AOceanGameMode>();
	TestNotNull(TEXT("[TDD] OceanHUD_GameModeCDO"), GameModeCDO);
	TestTrue(TEXT("[TDD] OceanHUD_GameModeHUDClass"), GameModeCDO->HUDClass.Get() == AOceanSurvivalHUD::StaticClass());

	return true;
}

#endif
