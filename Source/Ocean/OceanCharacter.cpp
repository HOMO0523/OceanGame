// Copyright Epic Games, Inc. All Rights Reserved.

#include "OceanCharacter.h"
#include "Ocean.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanInteractionComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanPaper2DAnimationComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "PaperFlipbookComponent.h"

AOceanCharacter::AOceanCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = false;
	GetCharacterMovement()->bSnapToPlaneAtStart = false;
	GetCharacterMovement()->JumpZVelocity = 420.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->GravityScale = 1.5f;

	// Create the camera boom component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	// Create the camera component
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	OceanInventoryComponent = CreateDefaultSubobject<UOceanInventoryComponent>(TEXT("OceanInventory"));
	OceanSurvivalComponent = CreateDefaultSubobject<UOceanSurvivalComponent>(TEXT("OceanSurvival"));
	OceanInteractionComponent = CreateDefaultSubobject<UOceanInteractionComponent>(TEXT("OceanInteraction"));
	OceanBuildComponent = CreateDefaultSubobject<UOceanBuildComponent>(TEXT("OceanBuild"));
	Paper2DVisualComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Paper2DVisual"));
	Paper2DVisualComponent->SetupAttachment(RootComponent);
	Paper2DVisualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, Paper2DVisualLandZ));
	Paper2DVisualComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	Paper2DVisualComponent->SetLooping(true);
	OceanPaper2DAnimationComponent = CreateDefaultSubobject<UOceanPaper2DAnimationComponent>(TEXT("OceanPaper2DAnimation"));

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AOceanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraBoom)
	{
		SurfaceCameraArmLength = CameraBoom->TargetArmLength;
	}
	RestoreSurfaceCameraState();
	ApplyPaper2DVisualStateOffset(Paper2DVisualLandZ);
}

void AOceanCharacter::ApplyDiveCameraState()
{
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = DiveCameraArmLength;
	}
}

void AOceanCharacter::RestoreSurfaceCameraState()
{
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = SurfaceCameraArmLength;
	}
}

void AOceanCharacter::ApplyPaper2DVisualStateOffset(float TargetZ)
{
	if (!Paper2DVisualComponent)
	{
		return;
	}

	FVector RelativeLocation = Paper2DVisualComponent->GetRelativeLocation();
	RelativeLocation.Z = TargetZ;
	Paper2DVisualComponent->SetRelativeLocation(RelativeLocation);

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanPaper2DVisualOffset: x=%.0f y=%.0f z=%.0f"),
		RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
}

FVector AOceanCharacter::ResolveDiveTargetLocation() const
{
	const FVector CurrentLocation = GetActorLocation();
	UWorld* World = GetWorld();
	if (!World)
	{
		return FVector(CurrentLocation.X, CurrentLocation.Y, DiveDepthZ);
	}

	const float CapsuleHalfHeight = GetCapsuleComponent() ? GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 96.0f;
	const FVector TraceStart(CurrentLocation.X, CurrentLocation.Y, WaterSurfaceZ + 100.0f);
	const FVector TraceEnd(CurrentLocation.X, CurrentLocation.Y, WaterSurfaceZ - DiveTraceDepth);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(OceanDiveTarget), false, this);
	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		const float FloorZ = Hit.ImpactPoint.Z + CapsuleHalfHeight + DiveFloorOffset;
		return FVector(CurrentLocation.X, CurrentLocation.Y, FloorZ);
	}

	return FVector(CurrentLocation.X, CurrentLocation.Y, DiveDepthZ);
}

void AOceanCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// If diving, stay in dive state — don't auto-switch movement mode
	if (bIsDiving)
	{
		ApplyPaper2DVisualStateOffset(Paper2DVisualDiveZ);
		if (OceanPaper2DAnimationComponent)
		{
			OceanPaper2DAnimationComponent->SetVisualStateRequest(EOceanPaper2DAnimationState::DiveSuitDive, true);
			OceanPaper2DAnimationComponent->UpdatePresentation(this, Paper2DVisualComponent.Get(), DeltaSeconds);
		}
		return;
	}

	// Detect water: character is in water when Z < WaterSurfaceZ (default 0).
	const float CharZ = GetActorLocation().Z;
	const bool bShouldSwim = CharZ < WaterSurfaceZ;

	UCharacterMovementComponent* MC = GetCharacterMovement();
	if (MC)
	{
		if (bShouldSwim && !bInWater)
		{
			MC->SetMovementMode(MOVE_Swimming);
			bInWater = true;
		}
		else if (!bShouldSwim && bInWater)
		{
			MC->SetMovementMode(MOVE_Walking);
			bInWater = false;
		}
	}

	// While in water (not diving), clamp Z to swim depth floor
	if (bInWater && MC && !bIsDiving)
	{
		FVector Loc = GetActorLocation();
		if (Loc.Z < SwimDepthFloor)
		{
			SetActorLocation(FVector(Loc.X, Loc.Y, SwimDepthFloor), false, nullptr, ETeleportType::TeleportPhysics);
			FVector V = MC->Velocity;
			if (V.Z < 0.0f) V.Z = 0.0f;
			MC->Velocity = V;
		}
	}

	if (OceanPaper2DAnimationComponent)
	{
		if (bInWater)
		{
			ApplyPaper2DVisualStateOffset(Paper2DVisualSwimZ);
			OceanPaper2DAnimationComponent->SetVisualStateRequest(EOceanPaper2DAnimationState::Swim, true);
		}
		else
		{
			ApplyPaper2DVisualStateOffset(Paper2DVisualLandZ);
			OceanPaper2DAnimationComponent->SetVisualStateRequest(EOceanPaper2DAnimationState::Swim, false);
		}

		OceanPaper2DAnimationComponent->UpdatePresentation(this, Paper2DVisualComponent.Get(), DeltaSeconds);
	}
}

bool AOceanCharacter::CanStartDiveAtCurrentLocation() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector ActorLocation = GetActorLocation();
	for (TActorIterator<AOceanFloatingPlatform> It(World); It; ++It)
	{
		if (It->IsWorldLocationAtWaterEdge(ActorLocation))
		{
			return true;
		}
	}

	return false;
}

bool AOceanCharacter::TryStartDive(FText& OutMessage)
{
	if (!CanStartDiveAtCurrentLocation())
	{
		OutMessage = NSLOCTEXT("Ocean", "DiveRequiresWaterEdge", "Need to be at water edge to dive");
		return false;
	}

	OutMessage = NSLOCTEXT("Ocean", "DiveEntryRequested", "Dive entry triggered");
	OnDiveRequested();
	return true;
}

bool AOceanCharacter::TryClimbPlatform()
{
	// Only works when in water (swimming)
	if (!bInWater || bIsDiving)
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanClimb: not in water, ignored"));
		return false;
	}

	UWorld* World = GetWorld();
	if (!World) return false;

	// Find nearest platform and check if player is near it
	for (TActorIterator<AOceanFloatingPlatform> It(World); It; ++It)
	{
		AOceanFloatingPlatform* Platform = *It;
		if (!Platform) continue;

		FVector PlayerLoc = GetActorLocation();
		FVector PlatformLoc = Platform->GetActorLocation();
		float DistXY = FVector::DistXY(PlayerLoc, PlatformLoc);

		// If within platform radius + some margin, snap player onto platform
		if (DistXY < 300.0f)
		{
			FVector NewLoc(PlatformLoc.X, PlatformLoc.Y, PlatformLoc.Z + PlatformTopOffset);
			SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);

			if (UCharacterMovementComponent* MC = GetCharacterMovement())
			{
				MC->SetMovementMode(MOVE_Walking);
				MC->Velocity = FVector::ZeroVector;
			}
			bInWater = false;
			ApplyPaper2DVisualStateOffset(Paper2DVisualLandZ);

			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanClimb: climbed to platform loc=(%.0f,%.0f,%.0f)"), NewLoc.X, NewLoc.Y, NewLoc.Z);
			return true;
		}
	}

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanClimb: no platform nearby"));
	return false;
}

bool AOceanCharacter::TryToggleDive()
{
	// Check inventory for diving suit
	UOceanInventoryComponent* Inventory = GetInventoryComponent();
	if (!Inventory || !Inventory->HasItem(FName(TEXT("dive_suit"))))
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDive: no dive suit in inventory"));
		return false;
	}

	if (!bIsDiving)
	{
		// Enter dive: must be swimming OR on platform + over water
		bool bCanDive = bInWater;

		if (!bCanDive)
		{
			// Check if on platform and over water (downward trace)
			UWorld* World = GetWorld();
			if (World)
			{
				FVector Start = GetActorLocation();
				FVector End = Start - FVector(0, 0, 500);
				FHitResult Hit;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(this);
				if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
				{
					// Hit platform = on platform; if platform is over water (Z near 0), can dive
					bCanDive = true;
				}
			}
		}

		if (!bCanDive)
		{
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDive: cannot dive here (not in water / not on platform over water)"));
			return false;
		}

		// Enter dive
		bIsDiving = true;
		bInWater = false;
		const FVector DiveTargetLocation = ResolveDiveTargetLocation();
		SetActorLocation(DiveTargetLocation, false, nullptr, ETeleportType::TeleportPhysics);

		if (UCharacterMovementComponent* MC = GetCharacterMovement())
		{
			MC->SetMovementMode(MOVE_Walking);
			MC->Velocity = FVector::ZeroVector;
		}

		ApplyDiveCameraState();
		ApplyPaper2DVisualStateOffset(Paper2DVisualDiveZ);

		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDive: entered dive at Z=%.0f camera_arm=%.0f"),
			DiveTargetLocation.Z, CameraBoom ? CameraBoom->TargetArmLength : -1.0f);
	}
	else
	{
		// Exit dive: rise to water surface
		bIsDiving = false;
		FVector Loc = GetActorLocation();
		SetActorLocation(FVector(Loc.X, Loc.Y, WaterSurfaceZ), false, nullptr, ETeleportType::TeleportPhysics);

		bInWater = true;
		if (UCharacterMovementComponent* MC = GetCharacterMovement())
		{
			MC->SetMovementMode(MOVE_Swimming);
			MC->Velocity = FVector::ZeroVector;
		}

		RestoreSurfaceCameraState();
		ApplyPaper2DVisualStateOffset(Paper2DVisualSwimZ);

		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDive: surfaced to Z=%.0f camera_arm=%.0f"),
			WaterSurfaceZ, CameraBoom ? CameraBoom->TargetArmLength : -1.0f);
	}

	return true;
}

void AOceanCharacter::ForceSurfaceAtSafeLocation(const FVector& SafeLocation)
{
	bIsDiving = false;
	bInWater = false;
	SetActorLocation(SafeLocation, false, nullptr, ETeleportType::TeleportPhysics);

	if (UCharacterMovementComponent* MC = GetCharacterMovement())
	{
		MC->SetMovementMode(MOVE_Walking);
		MC->Velocity = FVector::ZeroVector;
	}

	RestoreSurfaceCameraState();
	ApplyPaper2DVisualStateOffset(Paper2DVisualLandZ);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDive: force_surface_safe loc=(%.0f,%.0f,%.0f) camera_arm=%.0f"),
		SafeLocation.X, SafeLocation.Y, SafeLocation.Z, CameraBoom ? CameraBoom->TargetArmLength : -1.0f);
}

bool AOceanCharacter::TryFish()
{
	// Check inventory for fishing rod
	UOceanInventoryComponent* Inventory = GetInventoryComponent();
	if (!Inventory || !Inventory->HasItem(FName(TEXT("fishing_rod"))))
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanFish: no fishing rod in inventory"));
		return false;
	}

	// Must be on platform + over water (downward trace hits platform, platform is over water)
	UWorld* World = GetWorld();
	if (!World) return false;

	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 500);
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (!World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanFish: not standing on platform"));
		return false;
	}

	// Fishing success: gain food resource + small stamina cost
	if (UOceanSurvivalComponent* Survival = GetSurvivalComponent())
	{
		Survival->ApplyRecovery(-1.0f, 0.0f, 15.0f, 0.0f); // -1 stamina, +15 food
	}

	// Add fish to inventory
	if (Inventory)
	{
		FOceanItemStack FishStack;
		FishStack.ItemId = FName(TEXT("fish"));
		FishStack.Quantity = 1;
		FishStack.MaxStack = 10;
		FishStack.Category = EOceanItemCategory::Consumable;
		FishStack.UseEffect.SatietyDelta = 20.0f;
		Inventory->AddItem(FishStack);
	}

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanFish: caught a fish! food+15, fish added to inventory"));
	return true;
}
