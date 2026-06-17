#pragma once

#include "CoreMinimal.h"
#include "OceanItemTypes.generated.h"

UENUM(BlueprintType)
enum class EOceanItemCategory : uint8
{
	Resource UMETA(DisplayName = "Resource"),
	Consumable UMETA(DisplayName = "Consumable"),
	KeyItem UMETA(DisplayName = "Key Item"),
	Placeable UMETA(DisplayName = "Placeable")
};

UENUM(BlueprintType)
enum class EOceanInventoryDragDropResult : uint8
{
	Rejected UMETA(DisplayName = "Rejected"),
	Merged UMETA(DisplayName = "Merged"),
	Swapped UMETA(DisplayName = "Swapped"),
	Moved UMETA(DisplayName = "Moved")
};

USTRUCT(BlueprintType)
struct FOceanItemUseEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	float StaminaDelta = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	float HydrationDelta = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	float SatietyDelta = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	float HealthDelta = 0.0f;
};

USTRUCT(BlueprintType)
struct FOceanItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items", meta = (ClampMin = "0"))
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items", meta = (ClampMin = "1"))
	int32 MaxStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	EOceanItemCategory Category = EOceanItemCategory::Resource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	FOceanItemUseEffect UseEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	bool bKeyItem = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	bool bPlaceable = false;
};

USTRUCT(BlueprintType)
struct FOceanInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Inventory")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Inventory")
	FOceanItemStack Stack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Inventory")
	bool bLocked = false;
};
