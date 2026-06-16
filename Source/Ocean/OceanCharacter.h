// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OceanCharacter.generated.h"

class UCameraComponent;
class UOceanInteractionComponent;
class UOceanInventoryComponent;
class UOceanSurvivalComponent;
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

};

