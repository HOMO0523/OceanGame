#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanStatusPanelWidget::RebuildWidget()
{
	// ★ 必须在 RebuildWidget 中设置 RootWidget（NativeConstruct 太晚）
	RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
	WidgetTree->RootWidget = RootBox;

	auto MakeText = [this](FName Name) -> UTextBlock* {
		auto* T = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		T->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		T->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 18));
		RootBox->AddChildToVerticalBox(Cast<UWidget>(T))->SetPadding(FMargin(2));
		return T;
	};

	StaminaText = MakeText(TEXT("StaminaText"));
	HydrationText = MakeText(TEXT("HydrationText"));
	SatietyText = MakeText(TEXT("SatietyText"));

	UpdateVisuals();
	return Super::RebuildWidget();
}

void UOceanStatusPanelWidget::BindSurvivalComponent(UOceanSurvivalComponent* InSurvival)
{
	SurvivalComponent = InSurvival;
	if (SurvivalComponent.IsValid())
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanStatusPanel: bound=1 stamina=%.1f hydration=%.1f satiety=%.1f"),
			SurvivalComponent->GetStamina(), SurvivalComponent->GetHydration(), SurvivalComponent->GetSatiety());
		BroadcastIfChanged();
	}
	else
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanStatusPanel: bound=0 (null)"));
	}
}

void UOceanStatusPanelWidget::UnbindSurvivalComponent()
{
	SurvivalComponent = nullptr;
	LastStaminaPercent = LastHydrationPercent = LastSatietyPercent = -1.0f;
	LastStaminaCells = -1;
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
	BroadcastIfChanged();
}

void UOceanStatusPanelWidget::BroadcastIfChanged()
{
	if (!SurvivalComponent.IsValid()) return;
	const float SP = GetStaminaPercent(), HP = GetHydrationPercent(), SaP = GetSatietyPercent();
	const int32 SC = GetStaminaCells();

	if (!FMath::IsNearlyEqual(SP, LastStaminaPercent) || !FMath::IsNearlyEqual(HP, LastHydrationPercent) ||
		!FMath::IsNearlyEqual(SaP, LastSatietyPercent) || SC != LastStaminaCells)
	{
		LastStaminaPercent = SP; LastHydrationPercent = HP; LastSatietyPercent = SaP; LastStaminaCells = SC;
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanStatusPanel: updated stamina=%.1f hydration=%.1f satiety=%.1f cells=%d"),
			SurvivalComponent->GetStamina(), SurvivalComponent->GetHydration(), SurvivalComponent->GetSatiety(), SC);
		UpdateVisuals();
	}
}

void UOceanStatusPanelWidget::UpdateVisuals()
{
	if (StaminaText)
	{
		StaminaText->SetText(FText::FromString(
			FString::Printf(TEXT("体力: %d/3格 (%.0f%%)"), GetStaminaCells(), GetStaminaPercent() * 100.0f)));
		StaminaText->SetColorAndOpacity(GetStaminaPercent() < 0.34f ? FLinearColor::Red : FLinearColor::White);
	}
	if (HydrationText)
	{
		HydrationText->SetText(FText::FromString(
			FString::Printf(TEXT("水分: %.0f%%"), GetHydrationPercent() * 100.0f)));
		HydrationText->SetColorAndOpacity(GetHydrationPercent() < 0.3f ? FLinearColor::Red :
			(GetHydrationPercent() < 0.5f ? FLinearColor(1, 0.6f, 0) : FLinearColor(0.3f, 0.8f, 1.0f)));
	}
	if (SatietyText)
	{
		SatietyText->SetText(FText::FromString(
			FString::Printf(TEXT("饱食: %.0f%%"), GetSatietyPercent() * 100.0f)));
		SatietyText->SetColorAndOpacity(GetSatietyPercent() < 0.3f ? FLinearColor::Red :
			(GetSatietyPercent() < 0.5f ? FLinearColor(1, 0.6f, 0) : FLinearColor(1, 0.85f, 0.2f)));
	}
}
