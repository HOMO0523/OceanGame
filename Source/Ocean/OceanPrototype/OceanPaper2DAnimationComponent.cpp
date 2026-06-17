#include "OceanPrototype/OceanPaper2DAnimationComponent.h"

#include "OceanCharacter.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

UOceanPaper2DAnimationComponent::UOceanPaper2DAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOceanPaper2DAnimationComponent::UpdatePresentation(AOceanCharacter* Character, UPaperFlipbookComponent* VisualComponent, float DeltaSeconds)
{
	if (!Character || !VisualComponent)
	{
		return;
	}

	const FVector Velocity = Character->GetVelocity();
	CurrentDirection = DirectionFromWorldVector(Velocity, CurrentDirection);

	const UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
	const bool bIsFalling = MovementComponent && MovementComponent->IsFalling();
	const float GroundSpeed = Velocity.Size2D();

	CurrentState = bHasRequestedState
		? RequestedState
		: SelectAnimationState(false, false, false, bIsFalling, GroundSpeed);

	if (UWorld* World = Character->GetWorld())
	{
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0))
		{
			VisualComponent->SetWorldRotation(CalculateCameraFacingRotation(VisualComponent->GetComponentLocation(), CameraManager->GetCameraLocation()));
		}
	}

	if (UPaperFlipbook* TargetFlipbook = GetFlipbook(CurrentState, CurrentDirection))
	{
		if (VisualComponent->GetFlipbook() != TargetFlipbook)
		{
			VisualComponent->SetFlipbook(TargetFlipbook);
			VisualComponent->SetLooping(true);
			VisualComponent->Play();
		}
	}
}

void UOceanPaper2DAnimationComponent::SetVisualStateRequest(EOceanPaper2DAnimationState State, bool bEnabled)
{
	RequestedState = State;
	bHasRequestedState = bEnabled;
}

EOceanPaper2DDirection UOceanPaper2DAnimationComponent::DirectionFromWorldVector(const FVector& WorldVector, EOceanPaper2DDirection FallbackDirection, float DeadZone)
{
	if (WorldVector.SizeSquared2D() <= FMath::Square(DeadZone))
	{
		return FallbackDirection;
	}

	if (FMath::Abs(WorldVector.X) >= FMath::Abs(WorldVector.Y))
	{
		return WorldVector.X >= 0.0f ? EOceanPaper2DDirection::East : EOceanPaper2DDirection::West;
	}

	return WorldVector.Y >= 0.0f ? EOceanPaper2DDirection::North : EOceanPaper2DDirection::South;
}

EOceanPaper2DAnimationState UOceanPaper2DAnimationComponent::SelectAnimationState(bool bWantsDiveSuitDive, bool bWantsClimb, bool bWantsSwim, bool bIsFalling, float GroundSpeed, float WalkThreshold)
{
	if (bWantsDiveSuitDive)
	{
		return EOceanPaper2DAnimationState::DiveSuitDive;
	}

	if (bWantsClimb)
	{
		return EOceanPaper2DAnimationState::Climb;
	}

	if (bWantsSwim)
	{
		return EOceanPaper2DAnimationState::Swim;
	}

	if (bIsFalling)
	{
		return EOceanPaper2DAnimationState::Jump;
	}

	return GroundSpeed > WalkThreshold ? EOceanPaper2DAnimationState::Walk : EOceanPaper2DAnimationState::Idle;
}

FRotator UOceanPaper2DAnimationComponent::CalculateCameraFacingRotation(const FVector& VisualLocation, const FVector& CameraLocation)
{
	FVector ToCamera = CameraLocation - VisualLocation;
	ToCamera.Z = 0.0f;
	if (ToCamera.IsNearlyZero())
	{
		return FRotator::ZeroRotator;
	}

	return FRotator(0.0f, ToCamera.Rotation().Yaw, 0.0f);
}

UPaperFlipbook* UOceanPaper2DAnimationComponent::GetFlipbook(EOceanPaper2DAnimationState State, EOceanPaper2DDirection Direction) const
{
	switch (State)
	{
	case EOceanPaper2DAnimationState::Idle:
		switch (Direction)
		{
		case EOceanPaper2DDirection::West: return IdleWest.Get();
		case EOceanPaper2DDirection::East: return IdleEast.Get();
		case EOceanPaper2DDirection::North: return IdleNorth.Get();
		case EOceanPaper2DDirection::South:
		default: return IdleSouth.Get();
		}
	case EOceanPaper2DAnimationState::Walk:
		switch (Direction)
		{
		case EOceanPaper2DDirection::West: return WalkWest.Get();
		case EOceanPaper2DDirection::East: return WalkEast.Get();
		case EOceanPaper2DDirection::North: return WalkNorth.Get();
		case EOceanPaper2DDirection::South:
		default: return WalkSouth.Get();
		}
	case EOceanPaper2DAnimationState::Jump:
		switch (Direction)
		{
		case EOceanPaper2DDirection::West: return JumpWest.Get();
		case EOceanPaper2DDirection::East: return JumpEast.Get();
		case EOceanPaper2DDirection::North: return JumpNorth.Get();
		case EOceanPaper2DDirection::South:
		default: return JumpSouth.Get();
		}
	case EOceanPaper2DAnimationState::Swim:
		switch (Direction)
		{
		case EOceanPaper2DDirection::West: return SwimWest.Get();
		case EOceanPaper2DDirection::East: return SwimEast.Get();
		case EOceanPaper2DDirection::North: return SwimNorth.Get();
		case EOceanPaper2DDirection::South:
		default: return SwimSouth.Get();
		}
	case EOceanPaper2DAnimationState::Climb:
		switch (Direction)
		{
		case EOceanPaper2DDirection::West: return ClimbWest.Get();
		case EOceanPaper2DDirection::East: return ClimbEast.Get();
		case EOceanPaper2DDirection::North: return ClimbNorth.Get();
		case EOceanPaper2DDirection::South:
		default: return ClimbSouth.Get();
		}
	case EOceanPaper2DAnimationState::DiveSuitDive:
		switch (Direction)
		{
		case EOceanPaper2DDirection::West: return DiveSuitDiveWest.Get();
		case EOceanPaper2DDirection::East: return DiveSuitDiveEast.Get();
		case EOceanPaper2DDirection::North: return DiveSuitDiveNorth.Get();
		case EOceanPaper2DDirection::South:
		default: return DiveSuitDiveSouth.Get();
		}
	default:
		return nullptr;
	}
}
