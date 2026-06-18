#include "OceanPrototype/OceanSaveManager.h"
#include "OceanPrototype/OceanSaveGame.h"
#include "OceanPrototype/OceanMVPGameMode.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanAutoPlayComponent.h"
#include "OceanCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Misc/DateTime.h"

const FString UOceanSaveManager::SETTINGS_SLOT_NAME = TEXT("OceanSettings");

void UOceanSaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CacheSettingsFromDisk();
}

FString UOceanSaveManager::GetSlotName(int32 SlotIndex) const
{
	return FString::Printf(TEXT("OceanSlot%d"), FMath::Clamp(SlotIndex, 1, MAX_SLOTS));
}

bool UOceanSaveManager::DoesSlotExist(int32 SlotIndex) const
{
	return UGameplayStatics::DoesSaveGameExist(GetSlotName(SlotIndex), GetSlotUserIndex());
}

void UOceanSaveManager::SetActiveSnapshotSlot(int32 SlotIndex)
{
	ActiveSnapshotSlotIndex = FMath::Clamp(SlotIndex, 1, MAX_SLOTS);
	UE_LOG(LogTemp, Log, TEXT("[TDD] OceanSave: active_snapshot_slot=%d"), ActiveSnapshotSlotIndex);
}

bool UOceanSaveManager::GetSlotInfo(int32 SlotIndex, int32& OutDay, FString& OutTimestamp, int32& OutEventsProcessed) const
{
	if (!DoesSlotExist(SlotIndex)) return false;

	UOceanSaveGame* Save = Cast<UOceanSaveGame>(UGameplayStatics::LoadGameFromSlot(GetSlotName(SlotIndex), GetSlotUserIndex()));
	if (!Save) return false;

	OutDay = Save->CurrentDay;
	OutTimestamp = Save->Timestamp;
	OutEventsProcessed = Save->TotalEventsProcessed;
	return true;
}

bool UOceanSaveManager::SaveToSlot(int32 SlotIndex)
{
	UOceanSaveGame* Save = CaptureCurrentState();
	if (!Save) return false;

	Save->SaveSlotName = GetSlotName(SlotIndex);
	Save->Timestamp = FDateTime::Now().ToString(TEXT("%Y.%m.%d %H:%M"));

	bool bSuccess = UGameplayStatics::SaveGameToSlot(Save, Save->SaveSlotName, GetSlotUserIndex());

	UE_LOG(LogTemp, Log, TEXT("[TDD] OceanSave: save slot=%d day=%d success=%d events=%d"),
		SlotIndex, Save->CurrentDay, bSuccess ? 1 : 0, Save->TotalEventsProcessed);

	if (OnSaveCompleted.IsBound()) OnSaveCompleted.Broadcast(bSuccess);
	return bSuccess;
}

bool UOceanSaveManager::SaveDaySnapshotToSlot(int32 SlotIndex)
{
	const int32 TargetSlot = FMath::Clamp(SlotIndex, 1, MAX_SLOTS);
	const bool bSuccess = SaveToSlot(TargetSlot);
	UE_LOG(LogTemp, Log, TEXT("[TDD] OceanDaySnapshot: slot=%d success=%d"), TargetSlot, bSuccess ? 1 : 0);
	return bSuccess;
}

UOceanSaveGame* UOceanSaveManager::LoadFromSlot(int32 SlotIndex)
{
	if (!DoesSlotExist(SlotIndex)) return nullptr;

	UOceanSaveGame* Save = Cast<UOceanSaveGame>(UGameplayStatics::LoadGameFromSlot(GetSlotName(SlotIndex), GetSlotUserIndex()));

	UE_LOG(LogTemp, Log, TEXT("[TDD] OceanSave: load slot=%d day=%d success=%d"),
		SlotIndex, Save ? Save->CurrentDay : 0, Save ? 1 : 0);

	if (OnLoadCompleted.IsBound()) OnLoadCompleted.Broadcast(Save != nullptr, Save);
	return Save;
}

bool UOceanSaveManager::DeleteSlot(int32 SlotIndex)
{
	if (!DoesSlotExist(SlotIndex)) return false;
	return UGameplayStatics::DeleteGameInSlot(GetSlotName(SlotIndex), GetSlotUserIndex());
}

UOceanSaveGame* UOceanSaveManager::CaptureCurrentState()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	UOceanSaveGame* Save = NewObject<UOceanSaveGame>();
	AOceanMVPGameMode* GM = Cast<AOceanMVPGameMode>(World->GetAuthGameMode());

	if (GM)
	{
		Save->CurrentDay = GM->CurrentDay;
		Save->TimeOfDayInt = static_cast<int32>(GM->CurrentTimeOfDay);
		Save->TotalEventsProcessed = GM->TotalEventsProcessed;
		Save->EventsProcessedToday = GM->EventsProcessedToday;
		Save->EventTimer = GM->EventTimer;
		Save->GamePhaseInt = static_cast<int32>(GM->GamePhase);

		if (GM->GetAutoPlay())
		{
			Save->AutoplaySeed = GM->GetAutoPlay()->GetRandomSeed();
		}
	}

	// Find player pawn for stats + inventory
	APawn* PlayerPawn = World->GetFirstPlayerController() ? World->GetFirstPlayerController()->GetPawn() : nullptr;
	if (PlayerPawn)
	{
		if (UOceanSurvivalComponent* Survival = PlayerPawn->FindComponentByClass<UOceanSurvivalComponent>())
		{
			Save->Stamina = Survival->GetStamina();
			Save->Hydration = Survival->GetHydration();
			Save->Satiety = Survival->GetSatiety();
			Save->bHasDied = Survival->IsDead();
		}

		if (UOceanInventoryComponent* Inventory = PlayerPawn->FindComponentByClass<UOceanInventoryComponent>())
		{
			Save->ResourceStacks = Inventory->GetStacks();
			Save->ItemSlots = Inventory->GetSlots();
		}
	}

	// Find platform
	for (TActorIterator<AOceanFloatingPlatform> It(World); It; ++It)
	{
		Save->PlatformLocation = It->GetActorLocation();
		break;
	}

	// Settings
	Save->BGMVolume = CachedBGMVolume;
	Save->SFXVolume = CachedSFXVolume;

	return Save;
}

bool UOceanSaveManager::ApplySaveState(UOceanSaveGame* SaveGame)
{
	if (!SaveGame) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	AOceanMVPGameMode* GM = Cast<AOceanMVPGameMode>(World->GetAuthGameMode());
	if (GM)
	{
		GM->CurrentDay = SaveGame->CurrentDay;
		GM->CurrentTimeOfDay = SaveGame->GetTimeOfDay();
		GM->TotalEventsProcessed = SaveGame->TotalEventsProcessed;
		GM->EventsProcessedToday = SaveGame->EventsProcessedToday;
		GM->EventTimer = SaveGame->EventTimer;
		GM->GamePhase = SaveGame->GetGamePhase();

		if (GM->GetAutoPlay())
		{
			GM->GetAutoPlay()->SetRandomSeed(SaveGame->AutoplaySeed);
		}
	}

	APawn* PlayerPawn = World->GetFirstPlayerController() ? World->GetFirstPlayerController()->GetPawn() : nullptr;
	if (PlayerPawn)
	{
		if (UOceanSurvivalComponent* Survival = PlayerPawn->FindComponentByClass<UOceanSurvivalComponent>())
		{
			Survival->SetStats(SaveGame->Stamina, SaveGame->Hydration, SaveGame->Satiety);
		}

		if (UOceanInventoryComponent* Inventory = PlayerPawn->FindComponentByClass<UOceanInventoryComponent>())
		{
			Inventory->SetMaxSlots(12);
			Inventory->RestoreInventoryState(SaveGame->ResourceStacks, SaveGame->ItemSlots);
		}

		const FVector SafeSurfaceLocation(SaveGame->PlatformLocation.X, SaveGame->PlatformLocation.Y, SaveGame->PlatformLocation.Z + 100.0f);
		if (AOceanCharacter* OceanCharacter = Cast<AOceanCharacter>(PlayerPawn))
		{
			OceanCharacter->ForceSurfaceAtSafeLocation(SafeSurfaceLocation);
		}
		else
		{
			PlayerPawn->SetActorLocation(SafeSurfaceLocation);
		}
	}

	// Move platform
	for (TActorIterator<AOceanFloatingPlatform> It(World); It; ++It)
	{
		It->SetActorLocation(SaveGame->PlatformLocation);
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("[TDD] OceanSave: applied day=%d events=%d sta=%.0f hyd=%.0f"),
		SaveGame->CurrentDay, SaveGame->TotalEventsProcessed, SaveGame->Stamina, SaveGame->Hydration);

	return true;
}

void UOceanSaveManager::CacheSettingsFromDisk()
{
	if (UGameplayStatics::DoesSaveGameExist(SETTINGS_SLOT_NAME, 0))
	{
		UOceanSaveGame* SettingsSave = Cast<UOceanSaveGame>(UGameplayStatics::LoadGameFromSlot(SETTINGS_SLOT_NAME, 0));
		if (SettingsSave)
		{
			CachedBGMVolume = SettingsSave->BGMVolume;
			CachedSFXVolume = SettingsSave->SFXVolume;
			return;
		}
	}
	CachedBGMVolume = 0.8f;
	CachedSFXVolume = 1.0f;
}

void UOceanSaveManager::SetVolumes(float NewBGM, float NewSFX)
{
	CachedBGMVolume = FMath::Clamp(NewBGM, 0.0f, 1.0f);
	CachedSFXVolume = FMath::Clamp(NewSFX, 0.0f, 1.0f);
}

bool UOceanSaveManager::SaveSettings()
{
	UOceanSaveGame* SettingsSave = NewObject<UOceanSaveGame>();
	SettingsSave->SaveSlotName = SETTINGS_SLOT_NAME;
	SettingsSave->BGMVolume = CachedBGMVolume;
	SettingsSave->SFXVolume = CachedSFXVolume;
	SettingsSave->Timestamp = FDateTime::Now().ToString(TEXT("%Y.%m.%d %H:%M"));
	return UGameplayStatics::SaveGameToSlot(SettingsSave, SETTINGS_SLOT_NAME, 0);
}
