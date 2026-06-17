#pragma once

#include "CoreMinimal.h"
#include "OceanBuildPlacementTypes.generated.h"

UENUM(BlueprintType)
enum class EOceanPlacementFailureReason : uint8
{
	None,
	NoBuildMode,
	NoSelectedModule,
	NoTargetPlatform,
	InvalidWorldHit,
	OccupiedCell,
	DetachedFromPlatform,
	InsufficientResources
};

USTRUCT(BlueprintType)
struct FOceanPlacementQueryResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	bool bCanPlace = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	EOceanPlacementFailureReason FailureReason = EOceanPlacementFailureReason::InvalidWorldHit;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	FIntPoint AnchorCell = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	TArray<FIntPoint> FootprintCells;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	FVector SnappedWorldLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	int32 RotationQuarterTurns = 0;
};
