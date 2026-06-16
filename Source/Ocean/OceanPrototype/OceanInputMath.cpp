#include "OceanPrototype/OceanInputMath.h"

FVector FOceanInputMath::MakeCameraRelativeMoveDirection(const FRotator& ViewRotation, const FVector2D& InputVector)
{
	if (InputVector.IsNearlyZero())
	{
		return FVector::ZeroVector;
	}

	const FRotator YawRotation(0.0f, ViewRotation.Yaw, 0.0f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	return (Forward * InputVector.Y + Right * InputVector.X).GetClampedToMaxSize(1.0f);
}
