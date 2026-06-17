// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "OceanPrototype/OceanMVPGameMode.h"
#include "OceanPlayerController.generated.h"

class UNiagaraSystem;
class UOceanBuildComponent;
class UOceanHUDRootWidget;
class UInputMappingContext;
class UInputAction;
class UPathFollowingComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Player controller for a top-down perspective game.
 *  Implements point and click based controls
 */
UCLASS(abstract)
class AOceanPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Component used for moving along a NavMesh path. */
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
	TObjectPtr<UPathFollowingComponent> PathFollowingComponent;

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, Category="Input")
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UNiagaraSystem> FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SetDestinationTouchAction;

	UPROPERTY(EditAnywhere, Category="Input|Ocean")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input|Ocean")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category="Input|Ocean")
	TObjectPtr<UInputAction> ToggleBuildAction;

	UPROPERTY(EditAnywhere, Category="Input|Ocean")
	TObjectPtr<UInputAction> ToggleBackpackAction;

	UPROPERTY(EditAnywhere, Category="Input|Ocean")
	TObjectPtr<UInputAction> RotateBuildAction;

	UPROPERTY(EditAnywhere, Category="Input|Ocean")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category="Input|Ocean")
	TObjectPtr<UInputAction> DiveAction;

	UPROPERTY(EditAnywhere, Category="Ocean|UI")
	TSubclassOf<UOceanHUDRootWidget> HUDRootWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UOceanHUDRootWidget> HUDRootWidget;

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	/** Set to true if we're using touch input */
	uint32 bIsTouch : 1;

	/** True when CachedDestination was updated from a valid cursor/touch hit */
	uint32 bHasCachedDestination : 1;

	/** Saved location of the character movement destination */
	FVector CachedDestination;

	/** Time that the click input has been pressed */
	float FollowTime = 0.0f;

public:

	/** Constructor */
	AOceanPlayerController();

protected:

	virtual void BeginPlay() override;

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;
	
	/** Input handlers */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
	void OnMoveTriggered(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);
	void OnInteractTriggered(const FInputActionValue& Value);
	void TryOceanInteract();
	void OnToggleBuildTriggered(const FInputActionValue& Value);
	void OnToggleBackpackTriggered(const FInputActionValue& Value);
	void OnRotateBuildTriggered(const FInputActionValue& Value);
	void OnJumpStarted(const FInputActionValue& Value);
	void OnJumpCompleted(const FInputActionValue& Value);
	void OnDiveTriggered(const FInputActionValue& Value);

	// Game phase change handler (bound to GameMode delegate)
	UFUNCTION() void OnGamePhaseChanged(EOceanGamePhase Phase);

	/** Helper function to get the move destination */
	bool UpdateCachedDestination();
	UOceanBuildComponent* GetControlledPawnBuildComponent() const;
};


