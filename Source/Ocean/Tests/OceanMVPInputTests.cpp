#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInputMath.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPCameraRelativeMoveTest, "Ocean.MVP.Input.CameraRelativeMove", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPCameraRelativeMoveTest::RunTest(const FString& Parameters)
{
	const FRotator CameraYaw(0.0f, 45.0f, 0.0f);

	const FVector Forward = FOceanInputMath::MakeCameraRelativeMoveDirection(CameraYaw, FVector2D(0.0f, 1.0f));
	TestTrue(TEXT("W produces a normalized direction"), Forward.IsNearlyZero() == false);
	TestEqual(TEXT("W follows flattened camera forward X"), static_cast<int32>(FMath::RoundToInt(Forward.X * 100.0f)), 71);
	TestEqual(TEXT("W follows flattened camera forward Y"), static_cast<int32>(FMath::RoundToInt(Forward.Y * 100.0f)), 71);

	const FVector Right = FOceanInputMath::MakeCameraRelativeMoveDirection(CameraYaw, FVector2D(1.0f, 0.0f));
	TestEqual(TEXT("D follows flattened camera right X"), static_cast<int32>(FMath::RoundToInt(Right.X * 100.0f)), -71);
	TestEqual(TEXT("D follows flattened camera right Y"), static_cast<int32>(FMath::RoundToInt(Right.Y * 100.0f)), 71);

	const FVector Diagonal = FOceanInputMath::MakeCameraRelativeMoveDirection(CameraYaw, FVector2D(1.0f, 1.0f));
	TestTrue(TEXT("Diagonal movement is clamped to unit length"), Diagonal.Size() <= 1.001f);

	return true;
}

#endif
