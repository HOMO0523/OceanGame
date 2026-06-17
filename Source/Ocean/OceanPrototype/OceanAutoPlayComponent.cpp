#include "OceanPrototype/OceanAutoPlayComponent.h"
#include "OceanPrototype/OceanMVPGameMode.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "Ocean.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

UOceanAutoPlayComponent::UOceanAutoPlayComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOceanAutoPlayComponent::BeginPlay()
{
	Super::BeginPlay();
	if (RandomSeed != 0) RNG.Initialize(RandomSeed);
	else RNG.GenerateNewSeed();
	FindComponents();
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: init seed=%d"), RandomSeed);
}

void UOceanAutoPlayComponent::FindComponents()
{
	UWorld* World = GetWorld();
	if (!World) return;

	GameMode = Cast<AOceanMVPGameMode>(World->GetAuthGameMode());

	for (TActorIterator<APawn> It(World); It; ++It)
	{
		APawn* P = *It;
		if (!P) continue;
		Survival = P->FindComponentByClass<UOceanSurvivalComponent>();
		Inventory = P->FindComponentByClass<UOceanInventoryComponent>();
		if (Survival.IsValid()) break;
	}
}

EOceanEncounterType UOceanAutoPlayComponent::RollEncounter(EOceanTimeOfDay TimeOfDay)
{
	if (TimeOfDay == EOceanTimeOfDay::Night)
	{
		return RollNightEncounter();
	}
	return RollDayEncounter();
}

EOceanEncounterType UOceanAutoPlayComponent::RollDayEncounter()
{
	// Per ocean.docx: Island 50%, Storm 10%, DriftLoot 10%, Nothing 10%, Ship 10%, Current 10%
	const int32 Roll = RNG.RandRange(0, 99);
	if (Roll < 50) return EOceanEncounterType::Island;
	if (Roll < 60) return EOceanEncounterType::Storm;
	if (Roll < 70) return EOceanEncounterType::DriftLoot;
	if (Roll < 80) return EOceanEncounterType::Nothing;
	if (Roll < 90) return EOceanEncounterType::DistantShip;
	return EOceanEncounterType::CurrentConfluence;
}

EOceanEncounterType UOceanAutoPlayComponent::RollNightEncounter()
{
	// Night: Nothing 30%, PredatorFish 25%, Storm 15%, Ship 15%, Hypothermia 15%
	const int32 Roll = RNG.RandRange(0, 99);
	if (Roll < 30) return EOceanEncounterType::Nothing;
	if (Roll < 55) return EOceanEncounterType::PredatorFish;
	if (Roll < 70) return EOceanEncounterType::Storm;
	if (Roll < 85) return EOceanEncounterType::DistantShip;
	return EOceanEncounterType::Hypothermia;
}

UOceanAutoPlayComponent::EIslandType UOceanAutoPlayComponent::RollIslandType()
{
	// Normal 60%, Lighthouse 20%, Coral 10%, Cannibal 6%, Sky 4%
	const int32 Roll = RNG.RandRange(0, 99);
	if (Roll < 60) return EIslandType::Normal;
	if (Roll < 80) return EIslandType::Lighthouse;
	if (Roll < 90) return EIslandType::Coral;
	if (Roll < 96) return EIslandType::Cannibal;
	return EIslandType::Sky;
}

void UOceanAutoPlayComponent::ExecuteEncounter(EOceanEncounterType Encounter)
{
	FText Desc = GetEncounterDescription(Encounter);
	LastEncounterText = Desc;

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: encounter=%d desc=%s"),
		static_cast<int32>(Encounter), *Desc.ToString());

	switch (Encounter)
	{
	case EOceanEncounterType::Island:
	{
		EIslandType Island = RollIslandType();
		const TCHAR* IslandNames[] = { TEXT("Normal Island"), TEXT("Abandoned Lighthouse"), TEXT("Coral Island"), TEXT("Cannibal Island"), TEXT("Sky Island") };
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: island_type=%d name=%s"), static_cast<int32>(Island), IslandNames[static_cast<int32>(Island)]);

		// Coral island: full recovery
		if (Island == EIslandType::Coral && Survival.IsValid())
		{
			Survival->SetStats(100.0f, 100.0f, 100.0f);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: coral_island full_recovery"));
		}
		// Cannibal island: drain 20% stamina
		if (Island == EIslandType::Cannibal && Survival.IsValid())
		{
			Survival->ApplyDamage(20.0f);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: cannibal_island sta_damage=20"));
		}
		GiveRandomLoot();
		break;
	}
	case EOceanEncounterType::Storm:
		// Storm: needs rope or costs 1 stamina
		if (Survival.IsValid())
		{
			Survival->ApplyDamage(33.0f); // -1 stamina cell
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: storm sta_damage=33"));
		}
		break;
	case EOceanEncounterType::DriftLoot:
		GiveRandomLoot();
		break;
	case EOceanEncounterType::Nothing:
		// Nothing happens
		break;
	case EOceanEncounterType::DistantShip:
		// Distant ship: could be rescue (false ending) or nothing
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: distant_ship (need signal item)"));
		break;
	case EOceanEncounterType::CurrentConfluence:
		// Current: costs 1 stamina
		if (Survival.IsValid())
		{
			Survival->ApplyDamage(33.0f);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: current sta_damage=33"));
		}
		break;
	case EOceanEncounterType::PredatorFish:
		// Predator fish at night: minor damage
		if (Survival.IsValid())
		{
			Survival->ApplyDamage(15.0f);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: predator_fish sta_damage=15"));
		}
		break;
	case EOceanEncounterType::Hypothermia:
		// Hypothermia: moderate damage
		if (Survival.IsValid())
		{
			Survival->ApplyDamage(25.0f);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: hypothermia sta_damage=25"));
		}
		break;
	}
}

void UOceanAutoPlayComponent::GiveRandomLoot()
{
	if (!Inventory.IsValid()) return;

	// Give a random consumable item
	const int32 Roll = RNG.RandRange(0, 3);
	const TCHAR* ItemIds[] = { TEXT("canned_food"), TEXT("fresh_water"), TEXT("coconut"), TEXT("soda") };

	FOceanItemStack Stack;
	Stack.ItemId = ItemIds[Roll];
	Stack.Quantity = 1;
	Stack.MaxStack = 8;
	Stack.Category = EOceanItemCategory::Consumable;

	// Apply effects per ocean.docx
	switch (Roll)
	{
	case 0: Stack.UseEffect.SatietyDelta = 100.0f; break; // canned food
	case 1: Stack.UseEffect.HydrationDelta = 100.0f; break; // water
	case 2: Stack.UseEffect.SatietyDelta = 20.0f; Stack.UseEffect.HydrationDelta = 20.0f; break; // coconut
	case 3: Stack.UseEffect.StaminaDelta = 33.0f; Stack.UseEffect.HydrationDelta = 20.0f; break; // soda
	}

	bool bAdded = Inventory->AddItem(Stack);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: loot item=%s qty=1 added=%d"), *Stack.ItemId.ToString(), bAdded ? 1 : 0);
}

EOceanEncounterType UOceanAutoPlayComponent::DoSail()
{
	// Sail: costs 1 stamina, random encounter
	if (Survival.IsValid()) Survival->ApplyDamage(33.0f);
	EOceanEncounterType Enc = RollDayEncounter();
	ExecuteEncounter(Enc);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: sail encounter=%d"), static_cast<int32>(Enc));
	return Enc;
}

void UOceanAutoPlayComponent::DoRest()
{
	if (Survival.IsValid())
	{
		Survival->ApplyHeal(33.0f); // +1 stamina cell
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: rest sta_heal=33"));
	}
	LastEncounterText = FText::FromString(TEXT("Rested. Stamina +1 cell."));
}

bool UOceanAutoPlayComponent::DoFish()
{
	if (Survival.IsValid()) Survival->ApplyDamage(33.0f); // costs 1 stamina

	// Give fish (food +50%)
	if (Inventory.IsValid())
	{
		FOceanItemStack Fish;
		Fish.ItemId = TEXT("fish");
		Fish.Quantity = 1;
		Fish.MaxStack = 8;
		Fish.Category = EOceanItemCategory::Consumable;
		Fish.UseEffect.SatietyDelta = 50.0f;
		Inventory->AddItem(Fish);
	}

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: fish success sta=-1 food_gained"));
	LastEncounterText = FText::FromString(TEXT("Caught a fish! Food +50%"));
	return true;
}

bool UOceanAutoPlayComponent::DoDive()
{
	// Dive needs dive gear - check inventory for "dive_gear" item
	if (!Inventory.IsValid()) return false;

	bool bHasGear = false;
	for (const FOceanInventorySlot& Slot : Inventory->GetSlots())
	{
		if (Slot.Stack.ItemId == TEXT("dive_gear") && Slot.Stack.Quantity > 0)
		{
			bHasGear = true;
			break;
		}
	}

	if (!bHasGear)
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: dive FAILED no_gear"));
		LastEncounterText = FText::FromString(TEXT("Need dive gear to dive!"));
		return false;
	}

	if (Survival.IsValid()) Survival->ApplyDamage(33.0f);
	GiveRandomLoot();
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanAutoPlay: dive success sta=-1 loot_gained"));
	LastEncounterText = FText::FromString(TEXT("Dived and found loot!"));
	return true;
}

FText UOceanAutoPlayComponent::GetEncounterDescription(EOceanEncounterType Type) const
{
	switch (Type)
	{
	case EOceanEncounterType::Island: return FText::FromString(TEXT("An island appears on the horizon!"));
	case EOceanEncounterType::Storm: return FText::FromString(TEXT("A storm hits! Lost 1 stamina."));
	case EOceanEncounterType::DriftLoot: return FText::FromString(TEXT("Floating debris with supplies!"));
	case EOceanEncounterType::Nothing: return FText::FromString(TEXT("Nothing happens. The sea is calm."));
	case EOceanEncounterType::DistantShip: return FText::FromString(TEXT("A distant ship passes by..."));
	case EOceanEncounterType::CurrentConfluence: return FText::FromString(TEXT("Caught in a current! Lost 1 stamina."));
	case EOceanEncounterType::PredatorFish: return FText::FromString(TEXT("Predator fish circle the boat!"));
	case EOceanEncounterType::Hypothermia: return FText::FromString(TEXT("The cold night bites. Lost stamina."));
	default: return FText::GetEmpty();
	}
}
