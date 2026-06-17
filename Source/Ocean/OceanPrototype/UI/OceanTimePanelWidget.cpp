#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanTimePanelWidget::RebuildWidget()
{
	TimeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TimeText"));
	TimeText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	TimeText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16));
	WidgetTree->RootWidget = TimeText;
	RefreshText();
	return Super::RebuildWidget();
}

void UOceanTimePanelWidget::SetDay(int32 NewDay)
{
	NewDay = FMath::Max(1, NewDay);
	if (Day != NewDay) { Day = NewDay; RefreshText(); }
}
void UOceanTimePanelWidget::SetTimeOfDay(EOceanTimeOfDay NewTimeOfDay)
{
	if (TimeOfDay != NewTimeOfDay) { TimeOfDay = NewTimeOfDay; RefreshText(); }
}
void UOceanTimePanelWidget::SetDayAndTime(int32 NewDay, EOceanTimeOfDay NewTimeOfDay)
{
	NewDay = FMath::Max(1, NewDay);
	if (Day != NewDay || TimeOfDay != NewTimeOfDay) { Day = NewDay; TimeOfDay = NewTimeOfDay; RefreshText(); }
}

void UOceanTimePanelWidget::RefreshText()
{
	if (TimeText)
	{
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("第%d天 %s"), Day, ToDisplayName(TimeOfDay))));
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanTimePanel: day=%d time=%d"), Day, static_cast<int32>(TimeOfDay));
	}
}
