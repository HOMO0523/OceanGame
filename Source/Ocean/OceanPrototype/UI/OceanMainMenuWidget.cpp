#include "OceanPrototype/UI/OceanMainMenuWidget.h"
#include "OceanPrototype/UI/OceanSaveSlotWidget.h"
#include "OceanPrototype/UI/OceanSettingsWidget.h"
#include "OceanPrototype/OceanSaveManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Ocean.h"

	TSharedRef<SWidget> UOceanMainMenuWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	// Root: canvas with dark background
	auto* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	// Button container — centered
	auto* ButtonBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ButtonBox"));
	UCanvasPanelSlot* BoxSlot = RootCanvas->AddChildToCanvas(ButtonBox);
	// Anchor center
	BoxSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
	BoxSlot->SetPosition(FVector2D(-150.0f, -200.0f));
	BoxSlot->SetSize(FVector2D(300.0f, 400.0f));

	// Title
	auto* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
	Title->SetText(FText::FromString(TEXT("OCEAN")));
	Title->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.9f, 1.0f, 1.0f)));
	Title->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 64));
	Title->SetJustification(ETextJustify::Center);
	ButtonBox->AddChild(Title);

	// New Game button
	NewGameButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("NewGameButton"));
	auto* NGText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NGText"));
	NGText->SetText(FText::FromString(TEXT("New Game")));
	NGText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	NGText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20));
	NewGameButton->AddChild(NGText);
	ButtonBox->AddChild(NewGameButton);

	// Continue label
	auto* ContinueLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContinueLabel"));
	ContinueLabel->SetText(FText::FromString(TEXT("Continue")));
	ContinueLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.8f, 0.9f)));
	ContinueLabel->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16));
	ButtonBox->AddChild(ContinueLabel);

	// Slot list
	SlotList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SlotList"));
	ButtonBox->AddChild(SlotList);

	// Settings button
	SettingsButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SettingsButton"));
	auto* SText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SText"));
	SText->SetText(FText::FromString(TEXT("Settings")));
	SText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20));
	SettingsButton->AddChild(SText);
	ButtonBox->AddChild(SettingsButton);

	// Quit button
	QuitButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("QuitButton"));
	auto* QText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("QText"));
	QText->SetText(FText::FromString(TEXT("Quit")));
	QText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	QText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20));
	QuitButton->AddChild(QText);
	ButtonBox->AddChild(QuitButton);

	return Super::RebuildWidget();
}

void UOceanMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMainMenu: initialized"));
}

void UOceanMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddUniqueDynamic(this, &UOceanMainMenuWidget::OnNewGameClicked);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddUniqueDynamic(this, &UOceanMainMenuWidget::OnSettingsClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(this, &UOceanMainMenuWidget::OnQuitClicked);
	}

	RefreshSlots();

	const bool bNewGameBound = NewGameButton && NewGameButton->OnClicked.IsBound();
	const bool bSettingsBound = SettingsButton && SettingsButton->OnClicked.IsBound();
	const bool bQuitBound = QuitButton && QuitButton->OnClicked.IsBound();
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMainMenuBindings: new_game=%d settings=%d quit=%d slot_list=%d"),
		bNewGameBound ? 1 : 0,
		bSettingsBound ? 1 : 0,
		bQuitBound ? 1 : 0,
		SlotList ? 1 : 0);
}

void UOceanMainMenuWidget::RefreshSlots()
{
	if (!SlotList) return;
	SlotList->ClearChildren();

	for (int32 i = 1; i <= 3; ++i)
	{
		auto* SlotWidget = WidgetTree->ConstructWidget<UOceanSaveSlotWidget>(UOceanSaveSlotWidget::StaticClass());
		SlotWidget->SetSlotIndex(i);
		SlotWidget->OnLoadClicked.AddDynamic(this, &UOceanMainMenuWidget::OnSlotLoadClicked);
		SlotWidget->OnDeleteClicked.AddDynamic(this, &UOceanMainMenuWidget::OnSlotDeleteClicked);
		SlotWidget->RefreshInfo();
		SlotList->AddChild(SlotWidget);
	}
}

void UOceanMainMenuWidget::OnNewGameClicked()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMainMenu: NewGame clicked -> open L_WaterOcean"));

	// Set pending load flag to 0 (new game, no save)
	UOceanSaveSlotWidget::PendingLoadSlot = 0;

	// Open game level — don't RemoveFromParent first, the level change will clean up
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_WaterOcean")));
}

void UOceanMainMenuWidget::OnSettingsClicked()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMainMenu: Settings clicked"));

	auto* SettingsWidget = CreateWidget<UOceanSettingsWidget>(GetOwningPlayer(), UOceanSettingsWidget::StaticClass());
	if (SettingsWidget)
	{
		SettingsWidget->AddToViewport(60);
		if (APlayerController* PC = GetOwningPlayer())
		{
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(SettingsWidget->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}

void UOceanMainMenuWidget::OnQuitClicked()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMainMenu: Quit clicked"));
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UOceanMainMenuWidget::OnSlotLoadClicked(int32 SlotIndex)
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMainMenu: load slot=%d -> open L_WaterOcean"), SlotIndex);

	// Set pending load — PlayerController BeginPlay will apply it after level loads
	UOceanSaveSlotWidget::PendingLoadSlot = SlotIndex;
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_WaterOcean")));
}

void UOceanMainMenuWidget::OnSlotDeleteClicked(int32 SlotIndex)
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanMainMenu: delete slot=%d"), SlotIndex);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UOceanSaveManager* SM = GI->GetSubsystem<UOceanSaveManager>())
		{
			SM->DeleteSlot(SlotIndex);
		}
	}
	RefreshSlots();
}
