#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OceanResourceTypes.h"
#include "OceanBuildModuleDefinition.generated.h"

class AOceanBuildModuleActor;
class UStaticMesh;

UCLASS(BlueprintType)
class OCEAN_API UOceanBuildModuleDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	FIntPoint FootprintSize = FIntPoint(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	TArray<FOceanResourceStack> BuildCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	TObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	TSubclassOf<AOceanBuildModuleActor> ModuleActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	bool bRequiresAdjacency = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	bool bCanStartOnWater = false;

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	TArray<FIntPoint> GetFootprintCells(FIntPoint AnchorCell, int32 RotationQuarterTurns) const;
};
