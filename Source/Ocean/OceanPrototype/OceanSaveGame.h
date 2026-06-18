#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OceanItemTypes.h"
#include "OceanResourceTypes.h"
#include "OceanSaveGame.generated.h"

// Forward declarations to avoid circular includes
enum class EOceanTimeOfDay : uint8;
enum class EOceanGamePhase : uint8;

/**
 * Serializable game state for the Ocean MVP.
 * Stores player stats, inventory, world state, and progression.
 */
UCLASS()
class OCEAN_API UOceanSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UOceanSaveGame();

	// --- Meta ---
	UPROPERTY(VisibleAnywhere, Category = "Ocean|Save")
	FString SaveSlotName = TEXT("OceanSlot1");

	UPROPERTY(VisibleAnywhere, Category = "Ocean|Save")
	FString Timestamp;

	UPROPERTY(VisibleAnywhere, Category = "Ocean|Save")
	int32 SaveVersion = 1;

	// --- Progression ---
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Progress")
	int32 CurrentDay = 1;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Progress")
	int32 TimeOfDayInt = 0; // 0=Morning, 1=Afternoon, 2=Night

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Progress")
	int32 TotalEventsProcessed = 0;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Progress")
	int32 GamePhaseInt = 0; // 0=Playing, 1=Won, 2=Lost

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Progress")
	int32 AutoplaySeed = 0;

	// --- Player Stats ---
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Player")
	float Stamina = 100.0f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Player")
	float Hydration = 100.0f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Player")
	float Satiety = 100.0f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Player")
	bool bHasDied = false;

	// --- Inventory ---
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Inventory")
	TArray<FOceanResourceStack> ResourceStacks;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Inventory")
	TArray<FOceanInventorySlot> ItemSlots;

	// --- World ---
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|World")
	FVector PlatformLocation = FVector(0.0f, 0.0f, 10.0f);

	// --- Settings ---
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Settings")
	float BGMVolume = 0.8f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Ocean|Save|Settings")
	float SFXVolume = 1.0f;

	/** Convenience: convert TimeOfDayInt to enum */
	EOceanTimeOfDay GetTimeOfDay() const;

	/** Convenience: convert GamePhaseInt to enum */
	EOceanGamePhase GetGamePhase() const;
};
