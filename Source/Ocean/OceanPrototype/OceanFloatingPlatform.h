#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OceanFloatingPlatform.generated.h"

class UOceanBuildGridComponent;
class USceneComponent;

UCLASS(Blueprintable)
class OCEAN_API AOceanFloatingPlatform : public AActor
{
	GENERATED_BODY()

public:
	AOceanFloatingPlatform();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "Ocean|Platform")
	UOceanBuildGridComponent* GetBuildGrid() const { return BuildGrid; }

	UFUNCTION(BlueprintCallable, Category = "Ocean|Platform")
	void InitializeCorePlatform();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Platform")
	void SetInitialCoreSize(FIntPoint NewCoreSize);

	UFUNCTION(BlueprintPure, Category = "Ocean|Platform")
	bool IsWorldLocationAtWaterEdge(const FVector& WorldLocation) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Platform")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Platform")
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Platform")
	TObjectPtr<UOceanBuildGridComponent> BuildGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Platform")
	FIntPoint InitialCoreSize = FIntPoint(2, 2);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Platform")
	bool bEnableVisualBobbing = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Platform", meta = (EditCondition = "bEnableVisualBobbing"))
	float BobbingAmplitude = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Platform", meta = (EditCondition = "bEnableVisualBobbing"))
	float BobbingFrequency = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Platform|Drift")
	bool bEnableNorthDrift = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Platform|Drift", meta = (EditCondition = "bEnableNorthDrift"))
	float DriftSpeed = 50.0f;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Platform")
	void SetDriftEnabled(bool bEnabled) { bEnableNorthDrift = bEnabled; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Platform")
	bool IsDrifting() const { return bEnableNorthDrift; }

private:
	FVector VisualRootInitialLocation = FVector::ZeroVector;
};
