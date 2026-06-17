// Copyright Epic Games, Inc. All Rights Reserved.

#include "OceanCharacter.h"
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
	Paper2DVisualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
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

	// stub
}

void AOceanCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (OceanPaper2DAnimationComponent)
	{
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
		OutMessage = NSLOCTEXT("Ocean", "DiveRequiresWaterEdge", "需要在水边才能潜水");
		return false;
	}

	OutMessage = NSLOCTEXT("Ocean", "DiveEntryRequested", "潜水入口已触发");
	OnDiveRequested();
	return true;
}
