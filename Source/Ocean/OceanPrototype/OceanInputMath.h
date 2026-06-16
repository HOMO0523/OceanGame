#pragma once

#include "CoreMinimal.h"

struct FOceanInputMath
{
	static FVector MakeCameraRelativeMoveDirection(const FRotator& ViewRotation, const FVector2D& InputVector);
};
