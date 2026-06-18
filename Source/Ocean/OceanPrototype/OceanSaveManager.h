#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OceanSaveManager.generated.h"

class UOceanSaveGame;
enum class EOceanTimeOfDay : uint8;
enum class EOceanGamePhase : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveCompleted, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadCompleted, bool, bSuccess, UOceanSaveGame*, LoadedSave);

/**
 * Manages save/load operations and slot metadata for the Ocean game.
 */
UCLASS()
class OCEAN_API UOceanSaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static constexpr int32 MAX_SLOTS = 3;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Save current game state to a slot. Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Save")
	bool SaveToSlot(int32 SlotIndex);

	/** Load a save game from a slot. Returns the loaded save game or nullptr. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Save")
	UOceanSaveGame* LoadFromSlot(int32 SlotIndex);

	/** Delete a save slot. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Save")
	bool DeleteSlot(int32 SlotIndex);

	/** Check if a save exists in the given slot. */
	UFUNCTION(BlueprintPure, Category = "Ocean|Save")
	bool DoesSlotExist(int32 SlotIndex) const;

	/** Get save metadata (Day + Timestamp) for display without full load. Returns false if no save. */
	UFUNCTION(BlueprintPure, Category = "Ocean|Save")
	bool GetSlotInfo(int32 SlotIndex, int32& OutDay, FString& OutTimestamp, int32& OutEventsProcessed) const;

	/** Collect state from GameMode + PlayerPawn into a UOceanSaveGame object. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Save")
	UOceanSaveGame* CaptureCurrentState();

	/** Apply a loaded save game's state to the current world (GameMode + PlayerPawn). */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Save")
	bool ApplySaveState(UOceanSaveGame* SaveGame);

	/** Get the current cached settings (loaded from slot 1 or defaults). */
	UFUNCTION(BlueprintPure, Category = "Ocean|Save|Settings")
	float GetBGMVolume() const { return CachedBGMVolume; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Save|Settings")
	float GetSFXVolume() const { return CachedSFXVolume; }

	UFUNCTION(BlueprintCallable, Category = "Ocean|Save|Settings")
	void SetVolumes(float NewBGM, float NewSFX);

	/** Persist settings to a dedicated settings slot. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Save|Settings")
	bool SaveSettings();

	UPROPERTY(BlueprintAssignable, Category = "Ocean|Save")
	FOnSaveCompleted OnSaveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Ocean|Save")
	FOnLoadCompleted OnLoadCompleted;

private:
	FString GetSlotName(int32 SlotIndex) const;
	int32 GetSlotUserIndex() const { return 0; }

	void CacheSettingsFromDisk();

	float CachedBGMVolume = 0.8f;
	float CachedSFXVolume = 1.0f;

	static const FString SETTINGS_SLOT_NAME;
};
