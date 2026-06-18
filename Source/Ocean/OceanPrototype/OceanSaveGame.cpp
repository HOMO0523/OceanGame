#include "OceanPrototype/OceanSaveGame.h"
#include "OceanPrototype/OceanMVPGameMode.h"
#include "OceanPrototype/UI/OceanTimePanelWidget.h"

UOceanSaveGame::UOceanSaveGame()
{
	SaveSlotName = TEXT("OceanSlot1");
}

EOceanTimeOfDay UOceanSaveGame::GetTimeOfDay() const
{
	switch (TimeOfDayInt)
	{
	case 0: return EOceanTimeOfDay::Morning;
	case 1: return EOceanTimeOfDay::Afternoon;
	case 2: return EOceanTimeOfDay::Night;
	default: return EOceanTimeOfDay::Morning;
	}
}

EOceanGamePhase UOceanSaveGame::GetGamePhase() const
{
	switch (GamePhaseInt)
	{
	case 0: return EOceanGamePhase::Playing;
	case 1: return EOceanGamePhase::Won;
	case 2: return EOceanGamePhase::Lost;
	default: return EOceanGamePhase::Playing;
	}
}
