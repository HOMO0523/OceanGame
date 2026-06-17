#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "OceanBuildPlacementTypes.h"
#include "OceanBuildGridComponent.generated.h"

UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanBuildGridComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UOceanBuildGridComponent();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build Grid")
	void SetCellSize(float NewCellSize);

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	float GetCellSize() const { return CellSize; }

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build Grid")
	void SetGridOrigin(FVector NewGridOrigin);

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	FVector GetGridOrigin() const { return GridOrigin; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	FIntPoint WorldToCell(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	FVector CellToWorld(const FIntPoint& Cell) const;

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	static TArray<FIntPoint> BuildFootprint(FIntPoint AnchorCell, FIntPoint FootprintSize, int32 RotationQuarterTurns);

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	bool CanPlaceFootprint(const TArray<FIntPoint>& Footprint, bool bRequiresAdjacency) const;

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	EOceanPlacementFailureReason ExplainFootprintPlacement(const TArray<FIntPoint>& Footprint, bool bRequiresAdjacency) const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build Grid")
	void ReserveFootprint(const TArray<FIntPoint>& Footprint, FName OccupantId);

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	bool IsCellOccupied(FIntPoint Cell) const;

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	bool IsWaterAdjacentCell(FIntPoint Cell) const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build Grid")
	void ClearGrid();

	UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
	int32 GetOccupiedCellCount() const { return OccupiedCells.Num(); }

private:
	UPROPERTY(EditAnywhere, Category = "Ocean|Build Grid", meta = (ClampMin = "1.0"))
	float CellSize = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Build Grid")
	FVector GridOrigin = FVector::ZeroVector;

	TMap<FIntPoint, FName> OccupiedCells;

	static FIntPoint RotateLocalCell(FIntPoint LocalCell, FIntPoint FootprintSize, int32 NormalizedQuarterTurns);
	bool HasAdjacentOccupiedCell(const TArray<FIntPoint>& Footprint) const;
};
