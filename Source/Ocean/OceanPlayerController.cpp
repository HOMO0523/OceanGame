// Copyright Epic Games, Inc. All Rights Reserved.

#include "OceanPlayerController.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanInputMath.h"
#include "OceanPrototype/OceanInteractionComponent.h"
#include "OceanPrototype/UI/OceanHUDRootWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "InputCoreTypes.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "OceanCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Ocean.h"

AOceanPlayerController::AOceanPlayerController()
{
	bIsTouch = false;
	bMoveToMouseCursor = false;
	bHasCachedDestination = false;

	// create the path following comp
	PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(TEXT("Path Following Component"));

	// configure the controller
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AOceanPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() && HUDRootWidgetClass)
	{
		HUDRootWidget = CreateWidget<UOceanHUDRootWidget>(this, HUDRootWidgetClass);
		if (HUDRootWidget)
		{
			HUDRootWidget->AddToViewport();
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRootPIE: created=1 drawer_open=%d"), HUDRootWidget->IsBackpackOpen() ? 1 : 0);

			// 自动绑定 SurvivalComponent 到 StatusPanel
			APawn* ControlledPawn = GetPawn();
			if (ControlledPawn)
			{
				if (UOceanSurvivalComponent* Survival = ControlledPawn->FindComponentByClass<UOceanSurvivalComponent>())
				{
					HUDRootWidget->BindSurvivalToStatusPanel(Survival);
				}
			}
		}
	}
}

void AOceanPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Only set up input on local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// Setup mouse input events
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnInputStarted);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AOceanPlayerController::OnSetDestinationTriggered);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AOceanPlayerController::OnSetDestinationReleased);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AOceanPlayerController::OnSetDestinationReleased);

			// Setup touch input events
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnInputStarted);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AOceanPlayerController::OnTouchTriggered);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AOceanPlayerController::OnTouchReleased);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AOceanPlayerController::OnTouchReleased);

			if (MoveAction)
			{
				EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOceanPlayerController::OnMoveTriggered);
				EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AOceanPlayerController::OnMoveCompleted);
				EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AOceanPlayerController::OnMoveCompleted);
			}

			if (InteractAction)
			{
				EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnInteractTriggered);
			}

			if (ToggleBuildAction)
			{
				EnhancedInputComponent->BindAction(ToggleBuildAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnToggleBuildTriggered);
			}

			if (ToggleBackpackAction)
			{
				EnhancedInputComponent->BindAction(ToggleBackpackAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnToggleBackpackTriggered);
			}

			if (RotateBuildAction)
			{
				EnhancedInputComponent->BindAction(RotateBuildAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnRotateBuildTriggered);
			}

			if (JumpAction)
			{
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnJumpStarted);
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AOceanPlayerController::OnJumpCompleted);
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &AOceanPlayerController::OnJumpCompleted);
			}

			if (DiveAction)
			{
				EnhancedInputComponent->BindAction(DiveAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnDiveTriggered);
			}
		}
		else
		{
			UE_LOG(LogOcean, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
		}

		if (!InteractAction && InputComponent)
		{
			InputComponent->BindKey(EKeys::F, IE_Pressed, this, &AOceanPlayerController::TryOceanInteract);
		}
	}
}

void AOceanPlayerController::OnInputStarted()
{
	StopMovement();

	// Update the move destination to wherever the cursor is pointing at
	UpdateCachedDestination();
}

void AOceanPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// Update the move destination to wherever the cursor is pointing at
	const bool bDidUpdateDestination = UpdateCachedDestination();

	if (const UOceanBuildComponent* Build = GetControlledPawnBuildComponent())
	{
		if (Build->IsBuildModeActive())
		{
			return;
		}
	}

	if (!bDidUpdateDestination)
	{
		return;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AOceanPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		if (UOceanBuildComponent* Build = GetControlledPawnBuildComponent())
		{
			if (Build->IsBuildModeActive())
			{
				FText Message;
				if (bHasCachedDestination)
				{
					Build->TryPlaceSelectedModuleAtWorld(CachedDestination, Message);
				}
				else
				{
					Message = NSLOCTEXT("Ocean", "BuildNoValidPlacementCursor", "没有有效建造位置");
				}

				UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildResult: %s"), *Message.ToString());
				FollowTime = 0.f;
				return;
			}
		}

		if (bHasCachedDestination)
		{
			// We move there and spawn some particles
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AOceanPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AOceanPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AOceanPlayerController::OnMoveTriggered(const FInputActionValue& Value)
{
	StopMovement();

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	const FVector2D InputVector = Value.Get<FVector2D>();
	FRotator ViewRotation = GetControlRotation();
	FVector ViewLocation = FVector::ZeroVector;
	GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector WorldDirection = FOceanInputMath::MakeCameraRelativeMoveDirection(ViewRotation, InputVector);
	if (!WorldDirection.IsNearlyZero())
	{
		ControlledPawn->AddMovementInput(WorldDirection, 1.0f, false);
	}
}

void AOceanPlayerController::OnMoveCompleted(const FInputActionValue& Value)
{
}

void AOceanPlayerController::OnInteractTriggered(const FInputActionValue& Value)
{
	TryOceanInteract();
}

void AOceanPlayerController::TryOceanInteract()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	if (UOceanInteractionComponent* Interaction = ControlledPawn->FindComponentByClass<UOceanInteractionComponent>())
	{
		FText Message;
		Interaction->TryInteract(Message);
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanInteractResult: %s"), *Message.ToString());
	}
}

void AOceanPlayerController::OnToggleBuildTriggered(const FInputActionValue& Value)
{
	if (UOceanBuildComponent* Build = GetControlledPawnBuildComponent())
	{
		Build->ToggleBuildMode();
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildToggleHandled: result=PASS"));
	}
}

void AOceanPlayerController::OnToggleBackpackTriggered(const FInputActionValue& Value)
{
	if (!HUDRootWidget)
	{
		return;
	}

	HUDRootWidget->ToggleBackpack();

	const bool bBackpackOpen = HUDRootWidget->IsBackpackOpen();
	bShowMouseCursor = true;

	if (bBackpackOpen)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(HUDRootWidget->TakeWidget());
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}

void AOceanPlayerController::OnRotateBuildTriggered(const FInputActionValue& Value)
{
	if (UOceanBuildComponent* Build = GetControlledPawnBuildComponent())
	{
		Build->RotatePreview();
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildRotateHandled: result=PASS"));
	}
}

void AOceanPlayerController::OnJumpStarted(const FInputActionValue& Value)
{
	if (AOceanCharacter* OceanCharacter = Cast<AOceanCharacter>(GetPawn()))
	{
		OceanCharacter->Jump();
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanJumpHandled: result=PASS"));
	}
}

void AOceanPlayerController::OnJumpCompleted(const FInputActionValue& Value)
{
	if (AOceanCharacter* OceanCharacter = Cast<AOceanCharacter>(GetPawn()))
	{
		OceanCharacter->StopJumping();
	}
}

void AOceanPlayerController::OnDiveTriggered(const FInputActionValue& Value)
{
	if (AOceanCharacter* OceanCharacter = Cast<AOceanCharacter>(GetPawn()))
	{
		FText Message;
		const bool bStarted = OceanCharacter->TryStartDive(Message);
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDiveResult: allowed=%d message=%s"), bStarted ? 1 : 0, *Message.ToString());
	}
}

bool AOceanPlayerController::UpdateCachedDestination()
{
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	bHasCachedDestination = bHitSuccessful;

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	return bHasCachedDestination;
}

UOceanBuildComponent* AOceanPlayerController::GetControlledPawnBuildComponent() const
{
	const APawn* ControlledPawn = GetPawn();
	return ControlledPawn ? ControlledPawn->FindComponentByClass<UOceanBuildComponent>() : nullptr;
}
