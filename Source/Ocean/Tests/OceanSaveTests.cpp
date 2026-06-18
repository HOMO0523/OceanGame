#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanSaveManager.h"
#include "Engine/GameInstance.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanSaveManagerSnapshotSlotTest, "Ocean.MVP.Save.SnapshotSlotClamp", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanSaveManagerSnapshotSlotTest::RunTest(const FString& Parameters)
{
	UGameInstance* TestGameInstance = NewObject<UGameInstance>(GetTransientPackage());
	TestNotNull(TEXT("[TDD] OceanSave_TestGameInstance"), TestGameInstance);

	UOceanSaveManager* SaveManager = NewObject<UOceanSaveManager>(TestGameInstance, UOceanSaveManager::StaticClass());
	TestNotNull(TEXT("[TDD] OceanSave_SnapshotManagerCreated"), SaveManager);

	SaveManager->SetActiveSnapshotSlot(2);
	TestEqual(TEXT("[TDD] OceanSave_SnapshotSlotSet"), SaveManager->GetActiveSnapshotSlot(), 2);

	SaveManager->SetActiveSnapshotSlot(99);
	TestEqual(TEXT("[TDD] OceanSave_SnapshotSlotClampHigh"), SaveManager->GetActiveSnapshotSlot(), UOceanSaveManager::MAX_SLOTS);

	SaveManager->SetActiveSnapshotSlot(-5);
	TestEqual(TEXT("[TDD] OceanSave_SnapshotSlotClampLow"), SaveManager->GetActiveSnapshotSlot(), 1);
	return true;
}

#endif
