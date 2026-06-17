#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanPaper2DAnimationComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanPaper2DAnimationDirectionTest, "Ocean.Paper2D.Animation.Direction", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanPaper2DAnimationDirectionTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("[TDD] OceanPaper2D_Direction_WKey_Up"), UOceanPaper2DAnimationComponent::DirectionFromWorldVector(FVector(100.0f, 0.0f, 0.0f), EOceanPaper2DDirection::South), EOceanPaper2DDirection::North);
	TestEqual(TEXT("[TDD] OceanPaper2D_Direction_SKey_Down"), UOceanPaper2DAnimationComponent::DirectionFromWorldVector(FVector(-100.0f, 0.0f, 0.0f), EOceanPaper2DDirection::South), EOceanPaper2DDirection::South);
	TestEqual(TEXT("[TDD] OceanPaper2D_Direction_DKey_RightUsesWestRow"), UOceanPaper2DAnimationComponent::DirectionFromWorldVector(FVector(0.0f, 100.0f, 0.0f), EOceanPaper2DDirection::South), EOceanPaper2DDirection::West);
	TestEqual(TEXT("[TDD] OceanPaper2D_Direction_AKey_LeftUsesEastRow"), UOceanPaper2DAnimationComponent::DirectionFromWorldVector(FVector(0.0f, -100.0f, 0.0f), EOceanPaper2DDirection::North), EOceanPaper2DDirection::East);
	TestEqual(TEXT("[TDD] OceanPaper2D_Direction_ZeroKeepsLast"), UOceanPaper2DAnimationComponent::DirectionFromWorldVector(FVector::ZeroVector, EOceanPaper2DDirection::West), EOceanPaper2DDirection::West);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanPaper2DAnimationStatePriorityTest, "Ocean.Paper2D.Animation.StatePriority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanPaper2DAnimationStatePriorityTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("[TDD] OceanPaper2D_State_Idle"), UOceanPaper2DAnimationComponent::SelectAnimationState(false, false, false, false, 0.0f), EOceanPaper2DAnimationState::Idle);
	TestEqual(TEXT("[TDD] OceanPaper2D_State_Walk"), UOceanPaper2DAnimationComponent::SelectAnimationState(false, false, false, false, 220.0f), EOceanPaper2DAnimationState::Walk);
	TestEqual(TEXT("[TDD] OceanPaper2D_State_JumpBeatsWalk"), UOceanPaper2DAnimationComponent::SelectAnimationState(false, false, false, true, 220.0f), EOceanPaper2DAnimationState::Jump);
	TestEqual(TEXT("[TDD] OceanPaper2D_State_SwimBeatsJump"), UOceanPaper2DAnimationComponent::SelectAnimationState(false, false, true, true, 220.0f), EOceanPaper2DAnimationState::Swim);
	TestEqual(TEXT("[TDD] OceanPaper2D_State_ClimbBeatsSwim"), UOceanPaper2DAnimationComponent::SelectAnimationState(false, true, true, true, 220.0f), EOceanPaper2DAnimationState::Climb);
	TestEqual(TEXT("[TDD] OceanPaper2D_State_DiveBeatsClimb"), UOceanPaper2DAnimationComponent::SelectAnimationState(true, true, true, true, 220.0f), EOceanPaper2DAnimationState::DiveSuitDive);

	return true;
}

#endif
