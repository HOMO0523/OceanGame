#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanBuildGridComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

AOceanFloatingPlatform::AOceanFloatingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BuildGrid = CreateDefaultSubobject<UOceanBuildGridComponent>(TEXT("BuildGrid"));
	BuildGrid->SetupAttachment(SceneRoot);

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(SceneRoot);
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
