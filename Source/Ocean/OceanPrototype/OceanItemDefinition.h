#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanItemDefinition.generated.h"

class UTexture2D;

/**
 * UOceanItemDefinition
 * A UPrimaryDataAsset that defines a single item type in the Ocean survival game.
 * Designers create one DataAsset instance per item (e.g. "BottledWater", "FishSteak").
 * The definition holds all static metadata: identity, display info, category,
 * stacking rules, usability flags and the recovery effect applied on use.
 */
UCLASS(BlueprintType, Blueprintable)
class OCEAN_API UOceanItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UOceanItemDefinition();

	/** Stable identity used to match stacks. Should be unique across all definitions. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items")
	FName ItemId;

	/** Player-facing name shown in HUD / tooltips. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items")
	FText DisplayName;

	/** Player-facing description. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items", meta = (MultiLine = true))
	FText Description;

	/** Optional icon. Loaded softly to avoid holding texture memory until needed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Logical category. Drives inventory slot rules and use behaviour. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items")
	EOceanItemCategory Category = EOceanItemCategory::Resource;

	/** Maximum quantity per stack. Must be >= 1. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items", meta = (ClampMin = "1"))
	int32 MaxStack = 1;

	/** True if the item can be consumed via inventory TryUseItemAtSlot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items")
	bool bUsable = false;

	/** True if the item can be placed into the world as a construct. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items")
	bool bPlaceable = false;

	/** True if the item is a key / quest item that cannot be dropped or sold. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items")
	bool bKeyItem = false;

	/** Recovery effect applied when the item is consumed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean|Items")
	FOceanItemUseEffect UseEffect;

	/**
	 * Convert this definition into an FOceanItemStack with the given quantity.
	 * Convenience for spawning pickups / granting items from code.
	 */
	UFUNCTION(BlueprintPure, Category = "Ocean|Items")
	FOceanItemStack ToItemStack(int32 Quantity = 1) const;

	/** Convenience accessor used by the asset registry to bucket assets by type. */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
