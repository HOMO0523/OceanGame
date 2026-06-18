#include "OceanPrototype/OceanMVPGameMode.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanItemScatterComponent.h"
#include "OceanPrototype/OceanItemPickupActor.h"
#include "OceanPrototype/OceanDayNightCycleComponent.h"
#include "OceanPrototype/OceanAutoPlayComponent.h"
#include "OceanPrototype/OceanSaveManager.h"
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

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: BeginPlay (waiting for StartGame) total_events=%d"), GetTotalEventNodes());

	// Do NOT init game systems here — wait for player to click New Game / Continue.
}

void AOceanMVPGameMode::StartGame()
{
	if (bGameStarted) return;

	InitGameSystems();

	bGameStarted = true;
	GamePhase = EOceanGamePhase::Playing;

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: StartGame day=%d time=%d events=%d"),
		CurrentDay, static_cast<int32>(CurrentTimeOfDay), TotalEventsProcessed);
}

void AOceanMVPGameMode::InitGameSystems()
{
	if (bSystemsInitialized) return;
	bSystemsInitialized = true;

	// Find or create DayNightCycle component
	DayNightCycle = FindComponentByClass<UOceanDayNightCycleComponent>();
	if (!DayNightCycle)
	{
		DayNightCycle = NewObject<UOceanDayNightCycleComponent>(this, UOceanDayNightCycleComponent::StaticClass(), TEXT("DayNightCycle"));
		DayNightCycle->RegisterComponent();
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: created DayNightCycle"));
	}

	// Find or create AutoPlay component
	AutoPlay = FindComponentByClass<UOceanAutoPlayComponent>();
	if (!AutoPlay)
	{
		AutoPlay = NewObject<UOceanAutoPlayComponent>(this, UOceanAutoPlayComponent::StaticClass(), TEXT("AutoPlay"));
		AutoPlay->RegisterComponent();
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: created AutoPlay"));
	}

	// Apply initial lighting
	if (DayNightCycle)
	{
		DayNightCycle->SetTimeOfDay(CurrentTimeOfDay);
	}

	// Scatter initial items on the floating platform
	for (TActorIterator<AOceanFloatingPlatform> It(GetWorld()); It; ++It)
	{
		AOceanFloatingPlatform* Platform = *It;
		if (!Platform) continue;

		ScatterComp = Platform->FindComponentByClass<UOceanItemScatterComponent>();
		if (ScatterComp)
		{
			ScatterComp->ScatterItems(Platform->GetActorLocation(), 300.0f, 5);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: scattered items at platform (day %d)"), CurrentDay);
		}
		break;
	}

	// Give player starting items: fishing rod + dive suit
	if (APawn* PlayerPawn = GetWorld()->GetFirstPlayerController() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr)
	{
		if (UOceanInventoryComponent* Inventory = PlayerPawn->FindComponentByClass<UOceanInventoryComponent>())
		{
			FOceanItemStack FishingRod;
			FishingRod.ItemId = FName(TEXT("fishing_rod"));
			FishingRod.Quantity = 1;
			FishingRod.MaxStack = 1;
			FishingRod.Category = EOceanItemCategory::KeyItem;
			FishingRod.bKeyItem = true;
			Inventory->AddItem(FishingRod);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: gave fishing_rod to player"));

			FOceanItemStack DiveSuit;
			DiveSuit.ItemId = FName(TEXT("dive_suit"));
			DiveSuit.Quantity = 1;
			DiveSuit.MaxStack = 1;
			DiveSuit.Category = EOceanItemCategory::KeyItem;
			DiveSuit.bKeyItem = true;
			Inventory->AddItem(DiveSuit);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: gave dive_suit to player"));
		}
	}
}

void AOceanMVPGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Don't run game loop until the player has started the game
	if (!bGameStarted) return;
	if (GamePhase != EOceanGamePhase::Playing) return;

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

	// Apply survival drain per event node
	ApplyEventNodeSurvivalDrain();

	// Auto-play: roll random encounter for this time slot
	if (AutoPlay && Action == EOceanEventAction::None)
	{
		EOceanEncounterType Encounter = AutoPlay->RollEncounter(CurrentTimeOfDay);
		AutoPlay->ExecuteEncounter(Encounter);
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: encounter=%d"), static_cast<int32>(Encounter));
	}

	OnEventNodeProcessed.Broadcast(TotalEventsProcessed, Action);

	// Advance time and update lighting
	AdvanceTimeOfDay();

	if (DayNightCycle)
	{
		DayNightCycle->SetTimeOfDay(CurrentTimeOfDay);
	}

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

		// New day: clear yesterday's resources and scatter fresh ones around the player
		if (ScatterComp)
		{
			ScatterComp->ClearSpawnedPickups();

			FVector ScatterCenter = FVector::ZeroVector;
			if (APawn* PlayerPawn = GetWorld()->GetFirstPlayerController() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr)
			{
				ScatterCenter = PlayerPawn->GetActorLocation();
			}
			else
			{
				for (TActorIterator<AOceanFloatingPlatform> It(GetWorld()); It; ++It)
				{
					ScatterCenter = It->GetActorLocation();
					break;
				}
			}

			ScatterComp->ScatterItems(ScatterCenter, 400.0f, 5);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: daily scatter at (%.0f,%.0f,%.0f) day=%d"),
				ScatterCenter.X, ScatterCenter.Y, ScatterCenter.Z, CurrentDay);
		}

		if (UGameInstance* GameInstance = GetGameInstance())
		{
			if (UOceanSaveManager* SaveManager = GameInstance->GetSubsystem<UOceanSaveManager>())
			{
				const int32 SnapshotSlot = SaveManager->GetActiveSnapshotSlot();
				const bool bSnapshotSaved = SaveManager->SaveDaySnapshotToSlot(SnapshotSlot);
				UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDaySnapshot: slot=%d day=%d result=%d"),
					SnapshotSlot, CurrentDay, bSnapshotSaved ? 1 : 0);
			}
		}
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

		// Per ocean.docx: -20% hydration, -10% satiety per event node
		Survival->ApplyRecovery(0.0f, -20.0f, -10.0f, 0.0f);

		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMVPGameMode: drain sta=%.0f hyd=%.0f food=%.0f"),
			Survival->GetStamina(), Survival->GetHydration(), Survival->GetSatiety());
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
