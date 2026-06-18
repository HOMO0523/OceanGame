#include "OceanPrototype/UI/OceanSettingsWidget.h"
#include "OceanPrototype/OceanSaveManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "Engine/GameInstance.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanSettingsWidget::RebuildWidget()
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
	BoxSlot->SetSize(FVector2D(480.0f, 260.0f));

	// Title
	auto* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
	Title->SetText(FText::FromString(TEXT("Settings")));
	Title->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	Title->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24));
	Box->AddChild(Title);

	// BGM row
	auto* BGMRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("BGMRow"));
	auto* BGMLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BGMLabel"));
	BGMLabel->SetText(FText::FromString(TEXT("BGM Volume:")));
	BGMLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	BGMLabel->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16));
	BGMRow->AddChild(BGMLabel);

	BGMSlider = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass(), TEXT("BGMSlider"));
	BGMSlider->SetMinValue(0.0f);
	BGMSlider->SetMaxValue(1.0f);
	BGMRow->AddChild(BGMSlider);

	BGMValueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BGMValueText"));
	BGMValueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	BGMValueText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14));
	BGMRow->AddChild(BGMValueText);
	Box->AddChild(BGMRow);

	// SFX row
	auto* SFXRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("SFXRow"));
	auto* SFXLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SFXLabel"));
	SFXLabel->SetText(FText::FromString(TEXT("SFX Volume:")));
	SFXLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SFXLabel->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16));
	SFXRow->AddChild(SFXLabel);

	SFXSlider = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass(), TEXT("SFXSlider"));
	SFXSlider->SetMinValue(0.0f);
	SFXSlider->SetMaxValue(1.0f);
	SFXRow->AddChild(SFXSlider);

	SFXValueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SFXValueText"));
	SFXValueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SFXValueText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14));
	SFXRow->AddChild(SFXValueText);
	Box->AddChild(SFXRow);

	// Buttons
	auto* BtnRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("BtnRow"));
	SaveButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SaveButton"));
	auto* SaveText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SaveText"));
	SaveText->SetText(FText::FromString(TEXT("Save")));
	SaveText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SaveText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16));
	SaveButton->AddChild(SaveText);
	BtnRow->AddChild(SaveButton);

	CloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("CloseButton"));
	auto* CloseText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CloseText"));
	CloseText->SetText(FText::FromString(TEXT("Close")));
	CloseText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	CloseText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16));
	CloseButton->AddChild(CloseText);
	BtnRow->AddChild(CloseButton);
	Box->AddChild(BtnRow);

	return Super::RebuildWidget();
}

void UOceanSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanSettings: initialized"));
}

void UOceanSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Load current values
	float BGM = 0.8f, SFX = 1.0f;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UOceanSaveManager* SM = GI->GetSubsystem<UOceanSaveManager>())
		{
			BGM = SM->GetBGMVolume();
			SFX = SM->GetSFXVolume();
		}
	}

	if (BGMSlider)
	{
		BGMSlider->SetValue(BGM);
		BGMSlider->OnValueChanged.AddUniqueDynamic(this, &UOceanSettingsWidget::OnBGMChanged);
	}
	if (SFXSlider)
	{
		SFXSlider->SetValue(SFX);
		SFXSlider->OnValueChanged.AddUniqueDynamic(this, &UOceanSettingsWidget::OnSFXChanged);
	}
	if (SaveButton) SaveButton->OnClicked.AddUniqueDynamic(this, &UOceanSettingsWidget::OnSaveClicked);
	if (CloseButton) CloseButton->OnClicked.AddUniqueDynamic(this, &UOceanSettingsWidget::OnCloseClicked);

	UpdateValueLabels();

	const UWidget* Box = WidgetTree ? WidgetTree->FindWidget(TEXT("Box")) : nullptr;
	const UCanvasPanelSlot* BoxSlot = Box ? Cast<UCanvasPanelSlot>(Box->Slot) : nullptr;
	const bool bBoxCentered = BoxSlot
		&& BoxSlot->GetAnchors().Minimum.Equals(FVector2D(0.5f, 0.5f))
		&& BoxSlot->GetAnchors().Maximum.Equals(FVector2D(0.5f, 0.5f))
		&& BoxSlot->GetAlignment().Equals(FVector2D(0.5f, 0.5f))
		&& BoxSlot->GetPosition().Equals(FVector2D::ZeroVector);

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanSettingsBindings: bgm=%d sfx=%d save=%d close=%d centered=%d values=%.2f/%.2f"),
		(BGMSlider && BGMSlider->OnValueChanged.IsBound()) ? 1 : 0,
		(SFXSlider && SFXSlider->OnValueChanged.IsBound()) ? 1 : 0,
		(SaveButton && SaveButton->OnClicked.IsBound()) ? 1 : 0,
		(CloseButton && CloseButton->OnClicked.IsBound()) ? 1 : 0,
		bBoxCentered ? 1 : 0,
		BGM,
		SFX);
}

void UOceanSettingsWidget::OnBGMChanged(float Value)
{
	UpdateValueLabels();
}

void UOceanSettingsWidget::OnSFXChanged(float Value)
{
	UpdateValueLabels();
}

void UOceanSettingsWidget::UpdateValueLabels()
{
	if (BGMSlider && BGMValueText)
	{
		int32 Pct = FMath::RoundToInt(BGMSlider->GetValue() * 100.0f);
		BGMValueText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Pct)));
	}
	if (SFXSlider && SFXValueText)
	{
		int32 Pct = FMath::RoundToInt(SFXSlider->GetValue() * 100.0f);
		SFXValueText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Pct)));
	}
}

void UOceanSettingsWidget::OnSaveClicked()
{
	float BGM = BGMSlider ? BGMSlider->GetValue() : 0.8f;
	float SFX = SFXSlider ? SFXSlider->GetValue() : 1.0f;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UOceanSaveManager* SM = GI->GetSubsystem<UOceanSaveManager>())
		{
			SM->SetVolumes(BGM, SFX);
			SM->SaveSettings();
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanSettings: saved bgm=%.2f sfx=%.2f"), BGM, SFX);
		}
	}
}

void UOceanSettingsWidget::OnCloseClicked()
{
	RemoveFromParent();
}
