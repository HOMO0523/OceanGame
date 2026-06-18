// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OceanCharacter.generated.h"

class UCameraComponent;
class UOceanBuildComponent;
class UOceanInteractionComponent;
class UOceanInventoryComponent;
class UOceanPaper2DAnimationComponent;
class UOceanSurvivalComponent;
class UPaperFlipbookComponent;
class USpringArmComponent;

/**
 *  A controllable top-down perspective character
 */
UCLASS(abstract)
class AOceanCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanInventoryComponent> OceanInventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanSurvivalComponent> OceanSurvivalComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanInteractionComponent> OceanInteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanBuildComponent> OceanBuildComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPaperFlipbookComponent> Paper2DVisualComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanPaper2DAnimationComponent> OceanPaper2DAnimationComponent;

public:

	/** Constructor */
	AOceanCharacter();

	/** Initialization */
	virtual void BeginPlay() override;

	/** Update */
	virtual void Tick(float DeltaSeconds) override;

	/** Returns the camera component **/
	UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }

	/** Returns the Camera Boom component **/
	USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	UOceanInventoryComponent* GetInventoryComponent() const { return OceanInventoryComponent.Get(); }

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	UOceanSurvivalComponent* GetSurvivalComponent() const { return OceanSurvivalComponent.Get(); }

	UFUNCTION(BlueprintPure, Category = "Ocean|Interaction")
	UOceanInteractionComponent* GetInteractionComponent() const { return OceanInteractionComponent.Get(); }

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	UOceanBuildComponent* GetBuildComponent() const { return OceanBuildComponent.Get(); }

	UFUNCTION(BlueprintPure, Category = "Ocean|Visual")
	UPaperFlipbookComponent* GetPaper2DVisualComponent() const { return Paper2DVisualComponent.Get(); }

	UFUNCTION(BlueprintPure, Category = "Ocean|Visual")
	UOceanPaper2DAnimationComponent* GetPaper2DAnimationComponent() const { return OceanPaper2DAnimationComponent.Get(); }

	UFUNCTION(BlueprintPure, Category = "Ocean|Dive")
	bool CanStartDiveAtCurrentLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Dive")
	bool TryStartDive(FText& OutMessage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|Dive")
	void OnDiveRequested();

	/** Returns true if the character is currently in swimming mode. */
	UFUNCTION(BlueprintPure, Category = "Ocean|Water")
	bool IsInWater() const { return bInWater; }

	/** Returns true if the character is currently diving (deep underwater). */
	UFUNCTION(BlueprintPure, Category = "Ocean|Water")
	bool IsDiving() const { return bIsDiving; }

	/** Climb onto the platform from water (C key). Moves player to platform top. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Action")
	bool TryClimbPlatform();

	/** Toggle dive state (X key). Requires diving suit in inventory. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Action")
	bool TryToggleDive();

	/** Force a loaded game back to a safe surface/boat-side point and restore camera. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Action")
	void ForceSurfaceAtSafeLocation(const FVector& SafeLocation);

	/** Try fishing (key). Requires fishing rod in inventory. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Action")
	bool TryFish();

protected:
	/** Water surface Z. Character is "in water" when Z < this value. */
	UPROPERTY(EditAnywhere, Category = "Ocean|Water")
	float WaterSurfaceZ = 0.0f;

	/** Maximum depth the character can sink while swimming (Z floor). */
	UPROPERTY(EditAnywhere, Category = "Ocean|Water")
	float SwimDepthFloor = -110.0f;

	/** Camera arm length while underwater diving. */
	UPROPERTY(EditAnywhere, Category = "Ocean|Dive", meta = (ClampMin = "100.0"))
	float DiveCameraArmLength = 420.0f;

	/** How far below the water surface to look for an underwater floor. */
	UPROPERTY(EditAnywhere, Category = "Ocean|Dive", meta = (ClampMin = "100.0"))
	float DiveTraceDepth = 2000.0f;

	/** Capsule clearance over the underwater floor. */
	UPROPERTY(EditAnywhere, Category = "Ocean|Dive", meta = (ClampMin = "0.0"))
	float DiveFloorOffset = 8.0f;

private:
	/** Tracks whether the character is currently in swimming mode. */
	bool bInWater = false;

	/** Tracks whether the character is diving (deep underwater). */
	bool bIsDiving = false;

	/** Dive depth Z. */
	static constexpr float DiveDepthZ = -450.0f;
	/** Platform surface offset above platform Z. */
	static constexpr float PlatformTopOffset = 80.0f;

	float SurfaceCameraArmLength = 800.0f;

	FVector ResolveDiveTargetLocation() const;
	void ApplyDiveCameraState();
	void RestoreSurfaceCameraState();

};
