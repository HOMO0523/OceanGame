#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanCharacter.h"
#include "OceanPrototype/OceanInventoryComponent.h"

#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

namespace
{
UWorld* CreateDiveTestWorld()
{
	UGameInstance* TestGameInstance = NewObject<UGameInstance>(GEngine);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (World)
	{
		World->SetShouldTick(false);
		World->AddToRoot();

		FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
		WorldContext.OwningGameInstance = TestGameInstance;
		World->SetGameInstance(TestGameInstance);
		WorldContext.SetCurrentWorld(World);

		if (TestGameInstance)
		{
			TestGameInstance->Init();
		}
	}

	return World;
}

void DestroyDiveTestWorld(UWorld* World)
{
	if (!World)
	{
		return;
	}

	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		GameInstance->Shutdown();
	}

	if (GEngine)
	{
		GEngine->DestroyWorldContext(World);
	}
	World->RemoveFromRoot();
	World->DestroyWorld(false);
}

FOceanItemStack MakeDiveSuitForTest()
{
	FOceanItemStack DiveSuit;
	DiveSuit.ItemId = FName(TEXT("dive_suit"));
	DiveSuit.Quantity = 1;
	DiveSuit.MaxStack = 1;
	DiveSuit.Category = EOceanItemCategory::KeyItem;
	DiveSuit.bKeyItem = true;
	return DiveSuit;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanDiveCameraToggleTest, "Ocean.MVP.Dive.CameraToggle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanDiveCameraToggleTest::RunTest(const FString& Parameters)
{
	UWorld* World = CreateDiveTestWorld();
	TestNotNull(TEXT("[TDD] OceanDive_World"), World);
	if (!World)
	{
		return false;
	}

	UClass* CharacterClass = LoadClass<AOceanCharacter>(nullptr, TEXT("/Game/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter.BP_OceanSurvivorCharacter_C"));
	TestNotNull(TEXT("[TDD] OceanDive_CharacterClass"), CharacterClass);
	if (!CharacterClass)
	{
		DestroyDiveTestWorld(World);
		return false;
	}

	AOceanCharacter* Character = World->SpawnActor<AOceanCharacter>(CharacterClass, FVector(0.0f, 0.0f, -20.0f), FRotator::ZeroRotator);
	TestNotNull(TEXT("[TDD] OceanDive_Character"), Character);
	if (!Character)
	{
		DestroyDiveTestWorld(World);
		return false;
	}

	TestTrue(TEXT("[TDD] OceanDive_AddDiveSuit"), Character->GetInventoryComponent()->AddItem(MakeDiveSuitForTest()));
	Character->Tick(0.016f);

	USpringArmComponent* CameraBoom = Character->GetCameraBoom();
	TestNotNull(TEXT("[TDD] OceanDive_CameraBoom"), CameraBoom);
	if (!CameraBoom)
	{
		DestroyDiveTestWorld(World);
		return false;
	}

	const float SurfaceArmLength = CameraBoom->TargetArmLength;
	TestTrue(TEXT("[TDD] OceanDive_Enter"), Character->TryToggleDive());
	TestTrue(TEXT("[TDD] OceanDive_IsDiving"), Character->IsDiving());
	TestTrue(TEXT("[TDD] OceanDive_CameraArmShortened"), CameraBoom->TargetArmLength < SurfaceArmLength);
	TestEqual(TEXT("[TDD] OceanDive_WalksOnSeabed"), Character->GetCharacterMovement()->MovementMode, MOVE_Walking);

	TestTrue(TEXT("[TDD] OceanDive_Exit"), Character->TryToggleDive());
	TestFalse(TEXT("[TDD] OceanDive_NotDivingAfterExit"), Character->IsDiving());
	TestEqual(TEXT("[TDD] OceanDive_CameraArmRestored"), CameraBoom->TargetArmLength, SurfaceArmLength);

	DestroyDiveTestWorld(World);
	return true;
}

#endif
