#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "OceanMVPGameMode.generated.h"

class UOceanSurvivalComponent;
class UOceanInventoryComponent;
class AOceanFloatingPlatform;

UENUM(BlueprintType)
enum class EOceanGamePhase : uint8
{
	Playing    UMETA(DisplayName = "Playing"),
	Won        UMETA(DisplayName = "Won"),
	Lost       UMETA(DisplayName = "Lost")
};

UENUM(BlueprintType)
enum class EOceanEventAction : uint8
{
	Sail      UMETA(DisplayName = "Sail"),
	Dive      UMETA(DisplayName = "Dive"),
	Fish      UMETA(DisplayName = "Fish"),
	Rest      UMETA(DisplayName = "Rest"),
	None      UMETA(DisplayName = "None")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDayChangedSignature, int32, NewDay, EOceanTimeOfDay, TimeOfDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChangedSignature, EOceanGamePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEventNodeProcessedSignature, int32, EventIndex, EOceanEventAction, Action);

UCLASS()
class OCEAN_API AOceanMVPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOceanMVPGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Ocean|GameLoop")
	int32 CurrentDay = 1;

	UPROPERTY(EditAnywhere, Category = "Ocean|GameLoop")
	EOceanTimeOfDay CurrentTimeOfDay = EOceanTimeOfDay::Morning;

	UPROPERTY(EditAnywhere, Category = "Ocean|GameLoop")
	int32 EventsProcessedToday = 0;

	UPROPERTY(EditAnywhere, Category = "Ocean|GameLoop")
	int32 TotalEventsProcessed = 0;

	UPROPERTY(EditAnywhere, Category = "Ocean|GameLoop")
	int32 MaxDays = 7;

	UPROPERTY(EditAnywhere, Category = "Ocean|GameLoop")
	int32 EventsPerDay = 3;

	UPROPERTY(EditAnywhere, Category = "Ocean|GameLoop")
	float SecondsPerEventNode = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|GameLoop")
	float EventTimer = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|GameLoop")
	EOceanGamePhase GamePhase = EOceanGamePhase::Playing;

	UPROPERTY(BlueprintAssignable, Category = "Ocean|GameLoop")
	FOnDayChangedSignature OnDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ocean|GameLoop")
	FOnGamePhaseChangedSignature OnGamePhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ocean|GameLoop")
	FOnEventNodeProcessedSignature OnEventNodeProcessed;

	UFUNCTION(BlueprintCallable, Category = "Ocean|GameLoop")
	void ProcessEventNode(EOceanEventAction Action);

	UFUNCTION(BlueprintCallable, Category = "Ocean|GameLoop")
	void AdvanceTimeOfDay();

	UFUNCTION(BlueprintCallable, Category = "Ocean|GameLoop")
	void EndGame(EOceanGamePhase Phase);

	UFUNCTION(BlueprintPure, Category = "Ocean|GameLoop")
	int32 GetTotalEventNodes() const { return MaxDays * EventsPerDay; }

	UFUNCTION(BlueprintPure, Category = "Ocean|GameLoop")
	int32 GetRemainingEvents() const { return GetTotalEventNodes() - TotalEventsProcessed; }

protected:
	void ApplyEventNodeSurvivalDrain();
	void CheckGameEndConditions();
};
