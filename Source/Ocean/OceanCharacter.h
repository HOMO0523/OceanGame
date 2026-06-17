// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OceanCharacter.generated.h"

class UCameraComponent;
class UOceanBuildComponent;
class UOceanInteractionComponent;
class UOceanInventoryComponent;
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

	UFUNCTION(BlueprintPure, Category = "Ocean|Dive")
	bool CanStartDiveAtCurrentLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Dive")
	bool TryStartDive(FText& OutMessage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|Dive")
	void OnDiveRequested();

};

