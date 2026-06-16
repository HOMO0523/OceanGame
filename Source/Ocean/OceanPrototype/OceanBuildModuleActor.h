#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OceanBuildModuleActor.generated.h"

class UBuoyancyComponent;
class UOceanBuildModuleDefinition;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class OCEAN_API AOceanBuildModuleActor : public AActor
{
	GENERATED_BODY()

public:
	AOceanBuildModuleActor();

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	UBuoyancyComponent* GetBuoyancyComponent() const { return BuoyancyComponent; }

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	void SetPlaceholderMesh(UStaticMesh* NewMesh);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	void ConfigurePlacedModule(UOceanBuildModuleDefinition* InDefinition, const TArray<FIntPoint>& InOccupiedCells, int32 InRotationQuarterTurns);

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	const TArray<FIntPoint>& GetOccupiedCells() const { return OccupiedCells; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Water")
	TObjectPtr<UBuoyancyComponent> BuoyancyComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	TObjectPtr<UOceanBuildModuleDefinition> ModuleDefinition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	TArray<FIntPoint> OccupiedCells;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Build")
	int32 RotationQuarterTurns = 0;
};
