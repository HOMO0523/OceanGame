#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "Ocean.h"

void UOceanTimePanelWidget::SetDay(int32 NewDay)
{
	NewDay = FMath::Max(1, NewDay);
	if (Day != NewDay)
	{
		Day = NewDay;
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanTimePanel: day=%d time=%d"), Day, static_cast<int32>(TimeOfDay));
		OnTimeUpdated(Day, TimeOfDay);
	}
}

void UOceanTimePanelWidget::SetTimeOfDay(EOceanTimeOfDay NewTimeOfDay)
{
	if (TimeOfDay != NewTimeOfDay)
	{
		TimeOfDay = NewTimeOfDay;
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanTimePanel: day=%d time=%d"), Day, static_cast<int32>(TimeOfDay));
		OnTimeUpdated(Day, TimeOfDay);
	}
}

void UOceanTimePanelWidget::SetDayAndTime(int32 NewDay, EOceanTimeOfDay NewTimeOfDay)
{
	NewDay = FMath::Max(1, NewDay);
	if (Day != NewDay || TimeOfDay != NewTimeOfDay)
	{
		Day = NewDay;
		TimeOfDay = NewTimeOfDay;
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanTimePanel: day=%d time=%d"), Day, static_cast<int32>(TimeOfDay));
		OnTimeUpdated(Day, TimeOfDay);
	}
}
