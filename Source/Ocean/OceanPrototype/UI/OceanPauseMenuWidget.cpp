#include "OceanPrototype/UI/OceanPauseMenuWidget.h"
#include "OceanPrototype/UI/OceanSaveSlotWidget.h"
#include "OceanPrototype/UI/OceanSettingsWidget.h"
#include "OceanPrototype/OceanSaveManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanPauseMenuWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	auto* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
	WidgetTree->RootWidget = Root;

	auto* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Box"));
	UCanvasPanelSlot* BoxSlot = Root->AddChildToCanvas(Box);
	BoxSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
	BoxSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	BoxSlot->SetPosition(FVector2D::ZeroVector);
	BoxSlot->SetSize(FVector2D(460.0f, 440.0f));

	auto* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
	Title->SetText(FText::FromString(TEXT("Paused")));
	Title->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	Title->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 32));
	Box->AddChild(Title);

	// Save label
	auto* SaveLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SaveLabel"));
	SaveLabel->SetText(FText::FromString(TEXT("Save to slot:")));
	SaveLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.9f, 1.0f)));
	SaveLabel->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16));
	Box->AddChild(SaveLabel);

	SaveSlotList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SaveSlotList"));
	Box->AddChild(SaveSlotList);

	ResumeButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ResumeButton"));
	auto* RText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RText"));
	RText->SetText(FText::FromString(TEXT("Resume")));
	RText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	RText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20));
	ResumeButton->AddChild(RText);
	Box->AddChild(ResumeButton);

	SettingsButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SettingsButton"));
	auto* SText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SText"));
	SText->SetText(FText::FromString(TEXT("Settings")));
	SText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20));
	SettingsButton->AddChild(SText);
	Box->AddChild(SettingsButton);

	QuitToMenuButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("QuitToMenuButton"));
	auto* QText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("QText"));
	QText->SetText(FText::FromString(TEXT("Quit to Menu")));
	QText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	QText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20));
	QuitToMenuButton->AddChild(QText);
	Box->AddChild(QuitToMenuButton);

	return Super::RebuildWidget();
}

void UOceanPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanPauseMenu: initialized"));
}

void UOceanPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton) ResumeButton->OnClicked.AddUniqueDynamic(this, &UOceanPauseMenuWidget::OnResumeClicked);
	if (SettingsButton) SettingsButton->OnClicked.AddUniqueDynamic(this, &UOceanPauseMenuWidget::OnSettingsClicked);
	if (QuitToMenuButton) QuitToMenuButton->OnClicked.AddUniqueDynamic(this, &UOceanPauseMenuWidget::OnQuitToMenuClicked);

	BuildSaveSlots();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanPauseMenuBindings: resume=%d settings=%d quit=%d save_list=%d"),
		(ResumeButton && ResumeButton->OnClicked.IsBound()) ? 1 : 0,
		(SettingsButton && SettingsButton->OnClicked.IsBound()) ? 1 : 0,
		(QuitToMenuButton && QuitToMenuButton->OnClicked.IsBound()) ? 1 : 0,
		SaveSlotList ? 1 : 0);
}

void UOceanPauseMenuWidget::BuildSaveSlots()
{
	if (!SaveSlotList) return;
	SaveSlotList->ClearChildren();

	for (int32 i = 1; i <= 3; ++i)
	{
		auto* SlotWidget = WidgetTree->ConstructWidget<UOceanSaveSlotWidget>(UOceanSaveSlotWidget::StaticClass());
		SlotWidget->SetSlotIndex(i);
		// Reuse the load button as "save" by hooking into OnLoadClicked (acts as "select slot")
		SlotWidget->OnLoadClicked.AddUniqueDynamic(this, &UOceanPauseMenuWidget::OnSaveSlotClicked);
		SlotWidget->RefreshInfo();
		SaveSlotList->AddChild(SlotWidget);
	}
}

void UOceanPauseMenuWidget::OnResumeClicked()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanPauseMenu: resume"));
	RemoveFromParent();

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetPause(false);
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

void UOceanPauseMenuWidget::OnSettingsClicked()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanPauseMenu: settings"));
	auto* SettingsWidget = CreateWidget<UOceanSettingsWidget>(GetOwningPlayer(), UOceanSettingsWidget::StaticClass());
	if (SettingsWidget)
	{
		SettingsWidget->AddToViewport(60);
		if (APlayerController* PC = GetOwningPlayer())
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(SettingsWidget->TakeWidget());
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}

void UOceanPauseMenuWidget::OnQuitToMenuClicked()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanPauseMenu: quit to menu"));
	RemoveFromParent();

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetPause(false);
	}

	// Return to main menu
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_MainMenu")));
}

void UOceanPauseMenuWidget::OnSaveSlotClicked(int32 SlotIndex)
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanPauseMenu: save to slot=%d"), SlotIndex);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UOceanSaveManager* SM = GI->GetSubsystem<UOceanSaveManager>())
		{
			bool bOK = SM->SaveToSlot(SlotIndex);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanSave: result=%d"), bOK ? 1 : 0);
		}
	}

	// Refresh slot display
	BuildSaveSlots();
}
