#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanBuildGridComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"

AOceanFloatingPlatform::AOceanFloatingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BuildGrid = CreateDefaultSubobject<UOceanBuildGridComponent>(TEXT("BuildGrid"));
	BuildGrid->SetupAttachment(SceneRoot);

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(SceneRoot);

	// Visible platform deck — a flat cuboid placeholder, thin deck just above water
	PlatformCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlatformCollision"));
	PlatformCollision->SetupAttachment(VisualRoot);
	PlatformCollision->SetBoxExtent(FVector(200.0f, 200.0f, 10.0f));
	PlatformCollision->SetCollisionProfileName(TEXT("BlockAll"));
	PlatformCollision->SetMobility(EComponentMobility::Movable);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(PlatformCollision);
	PlatformMesh->SetCollisionProfileName(TEXT("NoCollision"));
	PlatformMesh->SetMobility(EComponentMobility::Movable);
	PlatformMesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 0.1f));

	// Use engine cube mesh as placeholder
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		PlatformMesh->SetStaticMesh(CubeMeshFinder.Object);
	}
}

void AOceanFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	VisualRootInitialLocation = VisualRoot->GetRelativeLocation();
	InitializeCorePlatform();
}

void AOceanFloatingPlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bEnableVisualBobbing)
	{
		const float BobOffset = FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f * UE_PI * BobbingFrequency) * BobbingAmplitude;
		VisualRoot->SetRelativeLocation(VisualRootInitialLocation + FVector(0.0f, 0.0f, BobOffset));
	}

	// Continuous north drift (Y+ in UE world space) — only when a pawn is standing on the deck.
	if (bEnableNorthDrift)
	{
		bool bPawnOnDeck = false;
		if (UWorld* World = GetWorld())
		{
			for (TActorIterator<APawn> It(World); It; ++It)
			{
				APawn* P = *It;
				if (P && IsWorldLocationOverPlatform(P->GetActorLocation()))
				{
					bPawnOnDeck = true;
					break;
				}
			}
		}

		if (bPawnOnDeck)
		{
			FVector CurrentLocation = GetActorLocation();
			FVector NewLocation = CurrentLocation + FVector(0.0f, DriftSpeed * DeltaSeconds, 0.0f);
			FHitResult Hit;
			SetActorLocation(NewLocation, true, &Hit);
		}
	}
}

void AOceanFloatingPlatform::InitializeCorePlatform()
{
	BuildGrid->ClearGrid();

	const FIntPoint SafeCoreSize(
		FMath::Max(1, InitialCoreSize.X),
		FMath::Max(1, InitialCoreSize.Y));
	const FIntPoint Anchor(
		-FMath::FloorToInt(static_cast<float>(SafeCoreSize.X) * 0.5f),
		-FMath::FloorToInt(static_cast<float>(SafeCoreSize.Y) * 0.5f));

	BuildGrid->ReserveFootprint(UOceanBuildGridComponent::BuildFootprint(Anchor, SafeCoreSize, 0), TEXT("CorePlatform"));
}

void AOceanFloatingPlatform::SetInitialCoreSize(FIntPoint NewCoreSize)
{
	InitialCoreSize = FIntPoint(FMath::Max(1, NewCoreSize.X), FMath::Max(1, NewCoreSize.Y));
}

bool AOceanFloatingPlatform::IsWorldLocationAtWaterEdge(const FVector& WorldLocation) const
{
	if (!BuildGrid)
	{
		return false;
	}

	return BuildGrid->IsWaterAdjacentCell(BuildGrid->WorldToCell(WorldLocation));
}

bool AOceanFloatingPlatform::IsWorldLocationOverPlatform(const FVector& WorldLocation, float ZTolerance) const
{
	const FVector MyLoc = GetActorLocation();

	// Platform deck half-extents (must match PlatformCollision box extent * scale)
	const float HalfX = 200.0f;
	const float HalfY = 200.0f;

	// X/Y bounds check
	if (FMath::Abs(WorldLocation.X - MyLoc.X) > HalfX) return false;
	if (FMath::Abs(WorldLocation.Y - MyLoc.Y) > HalfY) return false;

	// Z bounds check — allow pawn slightly above the deck (jumping) or at deck level
	if (FMath::Abs(WorldLocation.Z - MyLoc.Z) > ZTolerance) return false;

	return true;
}
