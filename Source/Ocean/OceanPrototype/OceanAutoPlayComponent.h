#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "OceanAutoPlayComponent.generated.h"

class AOceanMVPGameMode;
class UOceanSurvivalComponent;
class UOceanInventoryComponent;

/**
 * Auto-play system that triggers events per ocean.docx section 3.
 * Morning/Afternoon: random encounter (island 50%, storm 10%, drift loot 10%, nothing 10%, ship 10%, current 10%)
 * Night: random night event (nothing, predator fish, storm, ship, hypothermia)
 * Also handles player actions: Sail, Dive, Fish, Rest.
 */
UENUM(BlueprintType)
enum class EOceanEncounterType : uint8
{
	Island         UMETA(DisplayName = "Island"),
	Storm          UMETA(DisplayName = "Storm"),
	DriftLoot      UMETA(DisplayName = "Drift Loot"),
	Nothing        UMETA(DisplayName = "Nothing"),
	DistantShip    UMETA(DisplayName = "Distant Ship"),
	CurrentConfluence UMETA(DisplayName = "Current Confluence"),
	PredatorFish   UMETA(DisplayName = "Predator Fish"),
	Hypothermia    UMETA(DisplayName = "Hypothermia"),
};

UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanAutoPlayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanAutoPlayComponent();

	virtual void BeginPlay() override;

	/** Called by GameMode when an event node is processed. Picks a random encounter. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|AutoPlay")
	EOceanEncounterType RollEncounter(EOceanTimeOfDay TimeOfDay);

	/** Execute the encounter effect (damage, loot, toast, etc.) */
	UFUNCTION(BlueprintCallable, Category = "Ocean|AutoPlay")
	void ExecuteEncounter(EOceanEncounterType Encounter);

	/** Player action: Sail (costs 1 stamina, random encounter) */
	UFUNCTION(BlueprintCallable, Category = "Ocean|AutoPlay")
	EOceanEncounterType DoSail();

	/** Player action: Rest (gain 1 stamina) */
	UFUNCTION(BlueprintCallable, Category = "Ocean|AutoPlay")
	void DoRest();

	/** Player action: Fish (costs 1 stamina, gain food) */
	UFUNCTION(BlueprintCallable, Category = "Ocean|AutoPlay")
	bool DoFish();

	/** Player action: Dive (costs 1 stamina, needs dive gear) */
	UFUNCTION(BlueprintCallable, Category = "Ocean|AutoPlay")
	bool DoDive();

	UFUNCTION(BlueprintPure, Category = "Ocean|AutoPlay")
	FText GetEncounterDescription(EOceanEncounterType Type) const;

	UFUNCTION(BlueprintPure, Category = "Ocean|AutoPlay")
	FText GetLastEncounterText() const { return LastEncounterText; }

	UFUNCTION(BlueprintPure, Category = "Ocean|AutoPlay")
	int32 GetRandomSeed() const { return RandomSeed; }

	UFUNCTION(BlueprintCallable, Category = "Ocean|AutoPlay")
	void SetRandomSeed(int32 NewSeed) { RandomSeed = NewSeed; }

protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<AOceanMVPGameMode> GameMode;

	UPROPERTY(Transient)
	TWeakObjectPtr<UOceanSurvivalComponent> Survival;

	UPROPERTY(Transient)
	TWeakObjectPtr<UOceanInventoryComponent> Inventory;

	UPROPERTY(EditAnywhere, Category = "Ocean|AutoPlay|Seed")
	int32 RandomSeed = 0; // 0 = non-deterministic

	UPROPERTY(VisibleAnywhere, Category = "Ocean|AutoPlay")
	FText LastEncounterText;

private:
	FRandomStream RNG;

	void FindComponents();
	EOceanEncounterType RollDayEncounter();
	EOceanEncounterType RollNightEncounter();

	// Island sub-types per ocean.docx
	enum class EIslandType : uint8 { Normal, Lighthouse, Coral, Cannibal, Sky };
	EIslandType RollIslandType();

	// Give random loot to player
	void GiveRandomLoot();
};
