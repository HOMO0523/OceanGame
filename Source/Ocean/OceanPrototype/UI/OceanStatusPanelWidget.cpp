#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanStatusPanelWidget::RebuildWidget()
{
	if (bIsInitialized)
	{
		return Super::RebuildWidget();
	}
	bIsInitialized = true;

	// Root: dark semi-transparent background
	auto* BgBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BgBorder"));
	BgBorder->SetBrushColor(FLinearColor(0.02f, 0.04f, 0.08f, 0.75f));
	BgBorder->SetPadding(FMargin(8, 6));
	WidgetTree->RootWidget = BgBorder;

	RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
	BgBorder->AddChild(RootBox);

	auto MakeFont = [](int32 Size, bool bBold = false) -> FSlateFontInfo {
		FString FontPath = FPaths::EngineContentDir() / TEXT("Slate/Fonts/");
		FontPath /= bBold ? TEXT("Roboto-Bold.ttf") : TEXT("Roboto-Regular.ttf");
		return FSlateFontInfo(FontPath, Size);
	};

	// --- Row 1: STA label + 3 cell blocks ---
	auto* StaRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("StaRow"));
	RootBox->AddChildToVerticalBox(StaRow)->SetPadding(FMargin(0, 2));

	StaminaLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StaLabel"));
	StaminaLabel->SetText(FText::FromString(TEXT("STA")));
	StaminaLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 1.0f, 0.4f)));
	StaminaLabel->SetFont(MakeFont(14, true));
	StaRow->AddChildToHorizontalBox(StaminaLabel)->SetPadding(FMargin(0, 0, 6, 0));

	StaminaCellRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("StaCells"));
	StaRow->AddChildToHorizontalBox(StaminaCellRow);

	// Create 3 stamina cell blocks
	for (int32 i = 0; i < MaxStaminaCells; ++i)
	{
		auto* SizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), *FString::Printf(TEXT("CellSize%d"), i));
		SizeBox->SetWidthOverride(30.0f);
		SizeBox->SetHeightOverride(16.0f);

		auto* Cell = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), *FString::Printf(TEXT("Cell%d"), i));
		Cell->SetBrushColor(FLinearColor(0.1f, 0.5f, 0.1f, 1.0f)); // green filled
		Cell->SetPadding(FMargin(2));
		SizeBox->AddChild(Cell);

		auto* HSlot = StaminaCellRow->AddChildToHorizontalBox(SizeBox);
		HSlot->SetPadding(FMargin(1, 0));

		StaminaCells.Add(Cell);
	}

	// --- Helper: create a labeled progress bar row ---
	auto MakeBarRow = [&](FName LabelName, const FString& LabelText, const FLinearColor& LabelColor,
	                      FName BarName, TObjectPtr<UProgressBar>& OutBar, TObjectPtr<UTextBlock>& OutLabel) -> void
	{
		auto* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), LabelName);
		RootBox->AddChildToVerticalBox(Row)->SetPadding(FMargin(0, 1));

		OutLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *FString::Printf(TEXT("%s_Lbl"), *LabelText));
		OutLabel->SetText(FText::FromString(LabelText));
		OutLabel->SetColorAndOpacity(FSlateColor(LabelColor));
		OutLabel->SetFont(MakeFont(12, true));
		Row->AddChildToHorizontalBox(OutLabel)->SetPadding(FMargin(0, 0, 6, 0));

		auto* BarSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), *FString::Printf(TEXT("%s_Size"), *LabelText));
		BarSize->SetWidthOverride(120.0f);
		BarSize->SetHeightOverride(14.0f);

		UProgressBar* RawBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), BarName);
		RawBar->SetFillColorAndOpacity(LabelColor);
		RawBar->SetPercent(1.0f);
		BarSize->AddChild(RawBar);
		OutBar = RawBar;

		Row->AddChildToHorizontalBox(BarSize);
	};

	// --- Row 2: HYD bar ---
	MakeBarRow(TEXT("HydRow"), TEXT("HYD"), FLinearColor(0.3f, 0.7f, 1.0f), TEXT("HydBar"), HydrationBar, HydrationLabel);

	// --- Row 3: FOOD bar ---
	MakeBarRow(TEXT("FoodRow"), TEXT("FOOD"), FLinearColor(1.0f, 0.8f, 0.2f), TEXT("FoodBar"), SatietyBar, SatietyLabel);

	// --- Row 4: HP bar ---
	MakeBarRow(TEXT("HpRow"), TEXT("HP"), FLinearColor(1.0f, 0.2f, 0.2f), TEXT("HpBar"), HealthBar, HealthLabel);

	UpdateVisuals();
	return Super::RebuildWidget();
}

void UOceanStatusPanelWidget::BindSurvivalComponent(UOceanSurvivalComponent* InSurvival)
{
	SurvivalComponent = InSurvival;
	if (SurvivalComponent.IsValid())
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanStatusPanel: bound=1 sta=%.0f hyd=%.0f food=%.0f"),
			SurvivalComponent->GetStamina(), SurvivalComponent->GetHydration(), SurvivalComponent->GetSatiety());
		CheckForChanges();
	}
}

void UOceanStatusPanelWidget::UnbindSurvivalComponent()
{
	SurvivalComponent = nullptr;
	LastLoggedStaminaPct = LastLoggedHydrationPct = LastLoggedSatietyPct = -1;
}

float UOceanStatusPanelWidget::GetStaminaPercent() const
{
	return SurvivalComponent.IsValid() ? FMath::Clamp(SurvivalComponent->GetStamina() / 100.0f, 0.0f, 1.0f) : 0.0f;
}
float UOceanStatusPanelWidget::GetHydrationPercent() const
{
	return SurvivalComponent.IsValid() ? FMath::Clamp(SurvivalComponent->GetHydration() / 100.0f, 0.0f, 1.0f) : 0.0f;
}
float UOceanStatusPanelWidget::GetSatietyPercent() const
{
	return SurvivalComponent.IsValid() ? FMath::Clamp(SurvivalComponent->GetSatiety() / 100.0f, 0.0f, 1.0f) : 0.0f;
}
int32 UOceanStatusPanelWidget::GetStaminaCells() const
{
	if (!SurvivalComponent.IsValid()) return 0;
	const float S = SurvivalComponent->GetStamina();
	return S <= 0.0f ? 0 : FMath::Min(MaxStaminaCells, FMath::CeilToInt(S / StaminaPerCell));
}

void UOceanStatusPanelWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	CheckForChanges();
}

void UOceanStatusPanelWidget::CheckForChanges()
{
	if (!SurvivalComponent.IsValid()) return;

	// Use integer percent for comparison to throttle log spam
	const int32 StaPct = FMath::RoundToInt(GetStaminaPercent() * 100.0f);
	const int32 HydPct = FMath::RoundToInt(GetHydrationPercent() * 100.0f);
	const int32 FoodPct = FMath::RoundToInt(GetSatietyPercent() * 100.0f);
	const int32 Cells = GetStaminaCells();

	// Update visuals every frame (cheap), but only log when integer percent changes
	UpdateVisuals();

	if (StaPct != LastLoggedStaminaPct || HydPct != LastLoggedHydrationPct || FoodPct != LastLoggedSatietyPct)
	{
		LastLoggedStaminaPct = StaPct;
		LastLoggedHydrationPct = HydPct;
		LastLoggedSatietyPct = FoodPct;
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanStatusPanel: sta=%d%% hyd=%d%% food=%d%% cells=%d"),
			StaPct, HydPct, FoodPct, Cells);
	}
}

void UOceanStatusPanelWidget::UpdateVisuals()
{
	const int32 FilledCells = GetStaminaCells();

	// Stamina cells: filled = bright green, empty = dark gray
	for (int32 i = 0; i < StaminaCells.Num() && i < MaxStaminaCells; ++i)
	{
		if (StaminaCells[i])
		{
			if (i < FilledCells)
			{
				StaminaCells[i]->SetBrushColor(FLinearColor(0.1f, 0.7f, 0.1f, 1.0f));
			}
			else
			{
				StaminaCells[i]->SetBrushColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.8f));
			}
		}
	}

	// Stamina label color: red when low
	if (StaminaLabel)
	{
		StaminaLabel->SetColorAndOpacity(FilledCells == 0 ? FLinearColor::Red : FLinearColor(0.4f, 1.0f, 0.4f));
	}

	// Hydration bar
	if (HydrationBar)
	{
		HydrationBar->SetPercent(GetHydrationPercent());
		FLinearColor HydColor = GetHydrationPercent() < 0.2f ? FLinearColor::Red
			: GetHydrationPercent() < 0.4f ? FLinearColor(1.0f, 0.5f, 0.0f)
			: FLinearColor(0.3f, 0.7f, 1.0f);
		HydrationBar->SetFillColorAndOpacity(HydColor);
	}

	// Satiety bar
	if (SatietyBar)
	{
		SatietyBar->SetPercent(GetSatietyPercent());
		FLinearColor FoodColor = GetSatietyPercent() < 0.2f ? FLinearColor::Red
			: GetSatietyPercent() < 0.4f ? FLinearColor(1.0f, 0.5f, 0.0f)
			: FLinearColor(1.0f, 0.8f, 0.2f);
		SatietyBar->SetFillColorAndOpacity(FoodColor);
	}

	// Health bar (always 100% for now since Health not yet in SurvivalComponent)
	if (HealthBar)
	{
		HealthBar->SetPercent(1.0f);
	}
}
