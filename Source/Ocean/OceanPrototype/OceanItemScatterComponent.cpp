#include "OceanPrototype/OceanItemScatterComponent.h"
#include "OceanPrototype/OceanItemPickupActor.h"
#include "OceanPrototype/OceanItemDefinition.h"
#include "Engine/World.h"

UOceanItemScatterComponent::UOceanItemScatterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PickupActorClass = AOceanItemPickupActor::StaticClass();
}

TArray<AOceanItemPickupActor*> UOceanItemScatterComponent::GetLastSpawnedPickups() const
{
	TArray<AOceanItemPickupActor*> Result;
	for (const TObjectPtr<AOceanItemPickupActor>& P : LastSpawnedPickups)
	{
		if (P) Result.Add(P.Get());
	}
	return Result;
}

void UOceanItemScatterComponent::ClearSpawnedPickups()
{
	for (TObjectPtr<AOceanItemPickupActor>& P : LastSpawnedPickups)
	{
		if (P && IsValid(P))
		{
			P->Destroy();
		}
	}
	LastSpawnedPickups.Reset();
	UE_LOG(LogTemp, Log, TEXT("[TDD] OceanItemScatter: cleared spawned pickups"));
}

int32 UOceanItemScatterComponent::ScatterItems(FVector Center, float Radius, int32 Count)
{
	return ScatterItemsSeeded(Center, Radius, Count, DefaultSeed);
}

int32 UOceanItemScatterComponent::ScatterItemsSeeded(FVector Center, float Radius, int32 Count, int32 Seed)
{
	LastSpawnedPickups.Reset();

	if (Count <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[TDD] OceanItemScatter requested Count<=0, spawned 0"));
		return 0;
	}

	if (ItemPool.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TDD] OceanItemScatter ItemPool empty, spawned 0"));
		return 0;
	}

	if (!PickupActorClass)
	{
		PickupActorClass = AOceanItemPickupActor::StaticClass();
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TDD] OceanItemScatter no World, spawned 0"));
		return 0;
	}

	FRandomStream RandomStream(Seed);
	int32 Spawned = 0;

	for (int32 Index = 0; Index < Count; ++Index)
	{
		UOceanItemDefinition* Definition = ItemPool[Index % ItemPool.Num()];
		if (!Definition)
		{
			continue;
		}

		// Uniform disc sampling: r = R * sqrt(u), theta = 2*PI*v.
		const float U = RandomStream.FRand();
		const float V = RandomStream.FRand();
		const float SampleRadius = Radius * FMath::Sqrt(U);
		const float Angle = 2.0f * PI * V;
		const float X = Center.X + SampleRadius * FMath::Cos(Angle);
		const float Y = Center.Y + SampleRadius * FMath::Sin(Angle);
		const float Z = Center.Z + ZOffset;

		FVector SpawnLocation(X, Y, Z);
		FRotator SpawnRotation(0.0f, RandomStream.FRandRange(0.0f, 360.0f), 0.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AOceanItemPickupActor* Pickup = World->SpawnActor<AOceanItemPickupActor>(PickupActorClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (!Pickup)
		{
			UE_LOG(LogTemp, Warning, TEXT("[TDD] OceanItemScatter failed to spawn pickup at index %d"), Index);
			continue;
		}

		const int32 GrantedQuantity = FMath::Clamp(QuantityPerPickup, 1, FMath::Max(1, Definition->MaxStack));
		Pickup->InitializeFromDefinition(Definition, GrantedQuantity);

		LastSpawnedPickups.Add(Pickup);
		++Spawned;

		UE_LOG(LogTemp, Log, TEXT("[TDD] OceanItemScatter spawned %s at (%.2f, %.2f, %.2f) seed=%d"),
			*Definition->ItemId.ToString(), X, Y, Z, Seed);
	}

	UE_LOG(LogTemp, Log, TEXT("[TDD] OceanItemScatter complete: spawned %d / requested %d, seed=%d, center=(%.2f,%.2f,%.2f), radius=%.2f"),
		Spawned, Count, Seed, Center.X, Center.Y, Center.Z, Radius);

	return Spawned;
}
