#include "OceanPrototype/UI/OceanToastWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanToastWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	ToastBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ToastBorder"));
	ToastBorder->SetBrushColor(FLinearColor(0.02f, 0.08f, 0.02f, 0.9f));
	ToastBorder->SetPadding(FMargin(12, 6));
	WidgetTree->RootWidget = ToastBorder;

	ToastText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ToastText"));
	ToastText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ToastText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 14));
	ToastText->SetJustification(ETextJustify::Center);
	ToastBorder->AddChild(ToastText);

	SetVisibility(ESlateVisibility::Collapsed);
	return Super::RebuildWidget();
}

void UOceanToastWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(AutoDismissTimer);
	Super::NativeDestruct();
}

void UOceanToastWidget::Show(const FText& Message, float Duration)
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanToast: show=%s duration=%.1f"), *Message.ToString(), Duration);
	if (ToastText) ToastText->SetText(Message);
	SetVisibility(ESlateVisibility::Visible);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoDismissTimer);
		World->GetTimerManager().SetTimer(AutoDismissTimer, this, &UOceanToastWidget::OnAutoDismiss, Duration, false);
	}
}

void UOceanToastWidget::Hide()
{
	if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(AutoDismissTimer);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOceanToastWidget::OnAutoDismiss()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanToast: auto_dismiss"));
	SetVisibility(ESlateVisibility::Collapsed);
}
