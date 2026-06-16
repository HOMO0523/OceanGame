#include "OceanPrototype/OceanBuildModuleActor.h"
#include "BuoyancyComponent.h"
#include "Components/StaticMeshComponent.h"
#include "OceanPrototype/OceanBuildModuleDefinition.h"
#include "UObject/ConstructorHelpers.h"

AOceanBuildModuleActor::AOceanBuildModuleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	MeshComponent->SetSimulatePhysics(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	}

	BuoyancyComponent = CreateDefaultSubobject<UBuoyancyComponent>(TEXT("BuoyancyComponent"));
	BuoyancyComponent->AddCustomPontoon(100.0f, FVector(-75.0f, -75.0f, -40.0f));
	BuoyancyComponent->AddCustomPontoon(100.0f, FVector(75.0f, -75.0f, -40.0f));
	BuoyancyComponent->AddCustomPontoon(100.0f, FVector(-75.0f, 75.0f, -40.0f));
	BuoyancyComponent->AddCustomPontoon(100.0f, FVector(75.0f, 75.0f, -40.0f));
}

void AOceanBuildModuleActor::SetPlaceholderMesh(UStaticMesh* NewMesh)
{
	if (NewMesh)
	{
		MeshComponent->SetStaticMesh(NewMesh);
	}
}

void AOceanBuildModuleActor::ConfigurePlacedModule(UOceanBuildModuleDefinition* InDefinition, const TArray<FIntPoint>& InOccupiedCells, int32 InRotationQuarterTurns)
{
	ModuleDefinition = InDefinition;
	OccupiedCells = InOccupiedCells;
	RotationQuarterTurns = ((InRotationQuarterTurns % 4) + 4) % 4;
}
