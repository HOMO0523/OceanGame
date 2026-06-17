#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanTimePanelWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	auto* Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Bg"));
	Bg->SetBrushColor(FLinearColor(0.02f, 0.04f, 0.08f, 0.75f));
	Bg->SetPadding(FMargin(10, 4));
	WidgetTree->RootWidget = Bg;

	TimeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TimeText"));
	TimeText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	TimeText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16));
	TimeText->SetJustification(ETextJustify::Center);
	Bg->AddChild(TimeText);

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
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("Day %d - %s"), Day, ToDisplayName(TimeOfDay))));
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanTimePanel: day=%d time=%d"), Day, static_cast<int32>(TimeOfDay));
	}
}
