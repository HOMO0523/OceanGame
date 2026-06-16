#include "OceanPrototype/OceanBuildModuleDefinition.h"
#include "OceanPrototype/OceanBuildGridComponent.h"

TArray<FIntPoint> UOceanBuildModuleDefinition::GetFootprintCells(FIntPoint AnchorCell, int32 RotationQuarterTurns) const
{
	return UOceanBuildGridComponent::BuildFootprint(AnchorCell, FootprintSize, RotationQuarterTurns);
}
