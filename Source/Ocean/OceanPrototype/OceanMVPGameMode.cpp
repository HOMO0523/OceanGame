#include "OceanPrototype/OceanMVPGameMode.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanItemScatterComponent.h"
#include "OceanPrototype/OceanItemPickupActor.h"
#include "Ocean.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

AOceanMVPGameMode::AOceanMVPGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOceanMVPGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: start day=1 phase=Playing total_events=%d"), GetTotalEventNodes());

	// Scatter test items on the floating platform
	for (TActorIterator<AOceanFloatingPlatform> It(GetWorld()); It; ++It)
	{
		AOceanFloatingPlatform* Platform = *It;
		if (!Platform) continue;

		UOceanItemScatterComponent* Scatter = Platform->FindComponentByClass<UOceanItemScatterComponent>();
		if (Scatter)
		{
			Scatter->ScatterItems(Platform->GetActorLocation(), 300.0f, 5);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: scattered items at platform"));
		}
		break;
	}
}

void AOceanMVPGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GamePhase != EOceanGamePhase::Playing)
	{
		return;
	}

	EventTimer += DeltaSeconds;
	if (EventTimer >= SecondsPerEventNode)
	{
		EventTimer = 0.0f;
		ProcessEventNode(EOceanEventAction::None);
	}
}

void AOceanMVPGameMode::ProcessEventNode(EOceanEventAction Action)
{
	if (GamePhase != EOceanGamePhase::Playing) return;

	TotalEventsProcessed++;
	EventsProcessedToday++;

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: event=%d action=%d day=%d time=%d"),
		TotalEventsProcessed, static_cast<int32>(Action), CurrentDay,
		static_cast<int32>(CurrentTimeOfDay));

	ApplyEventNodeSurvivalDrain();

	switch (Action)
	{
	case EOceanEventAction::Rest:
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: rest → stamina+1"));
		break;
	case EOceanEventAction::Sail:
	case EOceanEventAction::Dive:
	case EOceanEventAction::Fish:
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: action=%d → stamina-1"), static_cast<int32>(Action));
		break;
	default:
		break;
	}

	OnEventNodeProcessed.Broadcast(TotalEventsProcessed, Action);
	AdvanceTimeOfDay();
	CheckGameEndConditions();
}

void AOceanMVPGameMode::AdvanceTimeOfDay()
{
	switch (CurrentTimeOfDay)
	{
	case EOceanTimeOfDay::Morning:
		CurrentTimeOfDay = EOceanTimeOfDay::Afternoon;
		break;
	case EOceanTimeOfDay::Afternoon:
		CurrentTimeOfDay = EOceanTimeOfDay::Night;
		break;
	case EOceanTimeOfDay::Night:
		CurrentTimeOfDay = EOceanTimeOfDay::Morning;
		CurrentDay++;
		EventsProcessedToday = 0;
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: new_day=%d"), CurrentDay);
		break;
	}

	OnDayChanged.Broadcast(CurrentDay, CurrentTimeOfDay);
}

void AOceanMVPGameMode::ApplyEventNodeSurvivalDrain()
{
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* Pawn = *It;
		if (!Pawn) continue;

		UOceanSurvivalComponent* Survival = Pawn->FindComponentByClass<UOceanSurvivalComponent>();
		if (!Survival) continue;

		Survival->ApplyRecovery(0.0f, -20.0f, -10.0f, 0.0f);

		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: drain sta=%.0f hyd=%.0f food=%.0f hp=%.0f"),
			Survival->GetStamina(), Survival->GetHydration(), Survival->GetSatiety(),
			Survival->GetHealth());
	}
}

void AOceanMVPGameMode::CheckGameEndConditions()
{
	if (TotalEventsProcessed >= GetTotalEventNodes())
	{
		EndGame(EOceanGamePhase::Won);
		return;
	}

	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* Pawn = *It;
		if (!Pawn) continue;

		UOceanSurvivalComponent* Survival = Pawn->FindComponentByClass<UOceanSurvivalComponent>();
		if (Survival && Survival->IsDead())
		{
			EndGame(EOceanGamePhase::Lost);
			return;
		}
	}
}

void AOceanMVPGameMode::EndGame(EOceanGamePhase Phase)
{
	if (GamePhase == Phase) return;

	GamePhase = Phase;
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: END phase=%d events=%d day=%d"),
		static_cast<int32>(Phase), TotalEventsProcessed, CurrentDay);

	OnGamePhaseChanged.Broadcast(Phase);
}
