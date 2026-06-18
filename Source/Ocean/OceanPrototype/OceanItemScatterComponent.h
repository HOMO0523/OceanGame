#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanItemScatterComponent.generated.h"

class UOceanItemDefinition;
class AOceanItemPickupActor;

/**
 * UOceanItemScatterComponent
 * Attaches to a FloatingPlatform or GameMode and scatters AOceanItemPickupActor
 * instances in a disc around a center point. Uses a deterministic FRandomStream
 * seed so tests can reproduce positions exactly.
 */
UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanItemScatterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanItemScatterComponent();

	/**
	 * Spawn Count pickup actors inside a disc of Radius around Center.
	 * Picks definitions round-robin from ItemPool. Each spawned pickup is
	 * offset slightly upward so it sits on top of the platform surface.
	 *
	 * @param Center     World-space center of the scatter disc.
	 * @param Radius     Disc radius in centimetres.
	 * @param Count      Total pickups to spawn.
	 * @return           Number of pickups actually spawned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Scatter")
	int32 ScatterItems(FVector Center, float Radius, int32 Count);

	/** Same as ScatterItems but with an explicit seed for deterministic tests. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Scatter")
	int32 ScatterItemsSeeded(FVector Center, float Radius, int32 Count, int32 Seed);

	/** Definitions to draw from when scattering. Round-robined in order. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Scatter")
	TArray<TObjectPtr<UOceanItemDefinition>> ItemPool;

	/** Pickup actor class to spawn. Defaults to AOceanItemPickupActor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Scatter")
	TSubclassOf<AOceanItemPickupActor> PickupActorClass;

	/** Seed used when ScatterItems is called without an explicit seed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Scatter")
	int32 DefaultSeed = 1337;

	/** Stack size granted per spawned pickup. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Scatter", meta = (ClampMin = "1"))
	int32 QuantityPerPickup = 1;

	/** Vertical offset added on top of Center so pickups sit on the platform. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Scatter")
	float ZOffset = 30.0f;

	/** Returns the actors spawned by the most recent scatter call. */
	UFUNCTION(BlueprintPure, Category = "Ocean|Scatter")
	TArray<AOceanItemPickupActor*> GetLastSpawnedPickups() const;

	/** Destroy all previously spawned pickup actors. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Scatter")
	void ClearSpawnedPickups();

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AOceanItemPickupActor>> LastSpawnedPickups;
};
