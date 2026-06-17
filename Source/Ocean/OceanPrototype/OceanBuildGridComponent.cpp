#include "OceanPrototype/OceanBuildGridComponent.h"

UOceanBuildGridComponent::UOceanBuildGridComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOceanBuildGridComponent::SetCellSize(float NewCellSize)
{
	CellSize = FMath::Max(1.0f, NewCellSize);
}

void UOceanBuildGridComponent::SetGridOrigin(FVector NewGridOrigin)
{
	GridOrigin = NewGridOrigin;
}

FIntPoint UOceanBuildGridComponent::WorldToCell(const FVector& WorldLocation) const
{
	const FVector Local = WorldLocation - GridOrigin;
	return FIntPoint(
		FMath::FloorToInt(Local.X / CellSize),
		FMath::FloorToInt(Local.Y / CellSize));
}

FVector UOceanBuildGridComponent::CellToWorld(const FIntPoint& Cell) const
{
	return GridOrigin + FVector(
		(static_cast<float>(Cell.X) + 0.5f) * CellSize,
		(static_cast<float>(Cell.Y) + 0.5f) * CellSize,
		0.0f);
}

TArray<FIntPoint> UOceanBuildGridComponent::BuildFootprint(FIntPoint AnchorCell, FIntPoint FootprintSize, int32 RotationQuarterTurns)
{
	TArray<FIntPoint> Result;
	const int32 Width = FMath::Max(0, FootprintSize.X);
	const int32 Height = FMath::Max(0, FootprintSize.Y);
	Result.Reserve(Width * Height);

	const int32 NormalizedTurns = ((RotationQuarterTurns % 4) + 4) % 4;
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			Result.Add(AnchorCell + RotateLocalCell(FIntPoint(X, Y), FIntPoint(Width, Height), NormalizedTurns));
		}
	}

	return Result;
}

bool UOceanBuildGridComponent::CanPlaceFootprint(const TArray<FIntPoint>& Footprint, bool bRequiresAdjacency) const
{
	if (Footprint.IsEmpty())
	{
		return false;
	}

	for (const FIntPoint& Cell : Footprint)
	{
		if (OccupiedCells.Contains(Cell))
		{
			return false;
		}
	}

	return !bRequiresAdjacency || HasAdjacentOccupiedCell(Footprint);
}

void UOceanBuildGridComponent::ReserveFootprint(const TArray<FIntPoint>& Footprint, FName OccupantId)
{
	for (const FIntPoint& Cell : Footprint)
	{
		OccupiedCells.Add(Cell, OccupantId);
	}
}

bool UOceanBuildGridComponent::IsCellOccupied(FIntPoint Cell) const
{
	return OccupiedCells.Contains(Cell);
}

bool UOceanBuildGridComponent::IsWaterAdjacentCell(FIntPoint Cell) const
{
	if (!OccupiedCells.Contains(Cell))
	{
		return false;
	}

	static const FIntPoint CardinalOffsets[] = {
		FIntPoint(1, 0),
		FIntPoint(-1, 0),
		FIntPoint(0, 1),
		FIntPoint(0, -1)
	};

	for (const FIntPoint& Offset : CardinalOffsets)
	{
		if (!OccupiedCells.Contains(Cell + Offset))
		{
			return true;
		}
	}

	return false;
}

void UOceanBuildGridComponent::ClearGrid()
{
	OccupiedCells.Reset();
}

FIntPoint UOceanBuildGridComponent::RotateLocalCell(FIntPoint LocalCell, FIntPoint FootprintSize, int32 NormalizedQuarterTurns)
{
	switch (NormalizedQuarterTurns)
	{
	case 1:
		return FIntPoint(LocalCell.Y, FootprintSize.X - 1 - LocalCell.X);
	case 2:
		return FIntPoint(FootprintSize.X - 1 - LocalCell.X, FootprintSize.Y - 1 - LocalCell.Y);
	case 3:
		return FIntPoint(FootprintSize.Y - 1 - LocalCell.Y, LocalCell.X);
	default:
		return LocalCell;
	}
}

bool UOceanBuildGridComponent::HasAdjacentOccupiedCell(const TArray<FIntPoint>& Footprint) const
{
	static const FIntPoint CardinalOffsets[] = {
		FIntPoint(1, 0),
		FIntPoint(-1, 0),
		FIntPoint(0, 1),
		FIntPoint(0, -1)
	};

	for (const FIntPoint& Cell : Footprint)
	{
		for (const FIntPoint& Offset : CardinalOffsets)
		{
			if (OccupiedCells.Contains(Cell + Offset))
			{
				return true;
			}
		}
	}

	return false;
}
