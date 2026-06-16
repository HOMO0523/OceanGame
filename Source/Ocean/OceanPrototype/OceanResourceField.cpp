#include "OceanPrototype/OceanResourceField.h"
#include "OceanPrototype/OceanResourceNode.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "PCGComponent.h"

AOceanResourceField::AOceanResourceField()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCGComponent"));
}

TArray<FVector> AOceanResourceField::GenerateFallbackResourceLocations(FVector Center) const
{
	TArray<FVector> Locations;
	Locations.Reserve(FallbackResourceCount);

	FRandomStream Stream(Seed);
	const float SafeFieldRadius = FMath::Max(FieldRadius, ExclusionRadius + 1.0f);
	const float MinRadiusSquared = FMath::Square(ExclusionRadius);
	const float MaxRadiusSquared = FMath::Square(SafeFieldRadius);

	for (int32 Index = 0; Index < FallbackResourceCount; ++Index)
	{
		const float Angle = Stream.FRandRange(0.0f, 2.0f * UE_PI);
		const float Radius = FMath::Sqrt(Stream.FRandRange(MinRadiusSquared, MaxRadiusSquared));
		Locations.Add(Center + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.0f));
	}

	return Locations;
}

void AOceanResourceField::SpawnFallbackResources(FVector Center)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TSubclassOf<AOceanResourceNode> SpawnClass = ResourceNodeClass;
	if (!SpawnClass)
	{
		SpawnClass = AOceanResourceNode::StaticClass();
	}

	for (AOceanResourceNode* ResourceNode : SpawnedResources)
	{
		if (ResourceNode)
		{
			ResourceNode->Destroy();
		}
	}
	SpawnedResources.Reset();

	for (const FVector& Location : GenerateFallbackResourceLocations(Center))
	{
		AOceanResourceNode* Spawned = World->SpawnActor<AOceanResourceNode>(SpawnClass, Location, FRotator::ZeroRotator);
		if (Spawned)
		{
			SpawnedResources.Add(Spawned);
		}
	}
}

void AOceanResourceField::SetFallbackResourceCount(int32 NewCount)
{
	FallbackResourceCount = FMath::Max(0, NewCount);
}

void AOceanResourceField::SetFieldRadius(float NewRadius)
{
	FieldRadius = FMath::Max(1.0f, NewRadius);
}

void AOceanResourceField::SetExclusionRadius(float NewRadius)
{
	ExclusionRadius = FMath::Max(0.0f, NewRadius);
}
