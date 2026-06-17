#include "OceanPrototype/UI/OceanItemTooltipWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanItemTooltipWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	TooltipBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("TooltipBorder"));
	TooltipBorder->SetBrushColor(FLinearColor(0.02f, 0.04f, 0.08f, 0.95f));
	TooltipBorder->SetPadding(FMargin(8, 5));
	WidgetTree->RootWidget = TooltipBorder;

	TooltipVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("TooltipVBox"));
	TooltipBorder->AddChild(TooltipVBox);

	auto MakeFont = [](int32 Size, bool bBold = false) -> FSlateFontInfo {
		FString FontPath = FPaths::EngineContentDir() / TEXT("Slate/Fonts/");
		FontPath /= bBold ? TEXT("Roboto-Bold.ttf") : TEXT("Roboto-Regular.ttf");
		return FSlateFontInfo(FontPath, Size);
	};

	NameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NameText"));
	NameText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 1.0f, 0.5f)));
	NameText->SetFont(MakeFont(14, true));
	TooltipVBox->AddChildToVerticalBox(NameText)->SetPadding(FMargin(0, 0, 0, 2));

	DescText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DescText"));
	DescText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
	DescText->SetFont(MakeFont(11));
	DescText->SetAutoWrapText(true);
	TooltipVBox->AddChildToVerticalBox(DescText)->SetPadding(FMargin(0, 0, 0, 2));

	EffectTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EffectText"));
	EffectTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.85f, 0.3f)));
	EffectTextBlock->SetFont(MakeFont(11));
	TooltipVBox->AddChildToVerticalBox(EffectTextBlock);

	SetVisibility(ESlateVisibility::Collapsed);
	return Super::RebuildWidget();
}

void UOceanItemTooltipWidget::ShowTooltip(const FText& ItemName, const FText& ItemDescription, const FText& EffectText)
{
	if (NameText) NameText->SetText(ItemName);
	if (DescText) DescText->SetText(ItemDescription);
	if (EffectTextBlock) EffectTextBlock->SetText(EffectText);
	SetVisibility(ESlateVisibility::Visible);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanItemTooltip: show=%s"), *ItemName.ToString());
}

void UOceanItemTooltipWidget::HideTooltip()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanItemTooltip: hide"));
}
