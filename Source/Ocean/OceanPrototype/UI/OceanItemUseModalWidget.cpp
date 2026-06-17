#include "OceanPrototype/UI/OceanItemUseModalWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanItemUseModalWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	ModalBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ModalBorder"));
	ModalBorder->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.1f, 0.95f));
	ModalBorder->SetPadding(FMargin(20, 15));
	WidgetTree->RootWidget = ModalBorder;

	auto* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	ModalBorder->AddChild(VBox);

	ItemNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ItemName"));
	ItemNameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ItemNameText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 18));
	ItemNameText->SetJustification(ETextJustify::Center);
	VBox->AddChildToVerticalBox(ItemNameText)->SetPadding(FMargin(0, 0, 0, 10));

	HintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HintText"));
	HintText->SetText(FText::FromString(TEXT("Use this item? [L-Click = Yes / R-Click = No]")));
	HintText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
	HintText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 13));
	HintText->SetJustification(ETextJustify::Center);
	VBox->AddChildToVerticalBox(HintText);

	SetVisibility(ESlateVisibility::Collapsed);
	return Super::RebuildWidget();
}

void UOceanItemUseModalWidget::ShowConfirmation(int32 InSlotIndex, const FOceanItemStack& InItem)
{
	PendingSlotIndex = InSlotIndex; PendingItem = InItem;
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanItemUseModal: show slot=%d item=%s"), InSlotIndex, *InItem.ItemId.ToString());
	if (ItemNameText) ItemNameText->SetText(FText::FromString(
		FString::Printf(TEXT("Use %s x%d?"), *InItem.ItemId.ToString(), InItem.Quantity)));
	SetVisibility(ESlateVisibility::Visible);
}
void UOceanItemUseModalWidget::Confirm()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanItemUseModal: confirmed slot=%d"), PendingSlotIndex);
	SetVisibility(ESlateVisibility::Collapsed);
	OnItemUseConfirmed.Broadcast(PendingSlotIndex);
	PendingSlotIndex = INDEX_NONE;
}
void UOceanItemUseModalWidget::Reject()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanItemUseModal: rejected slot=%d"), PendingSlotIndex);
	SetVisibility(ESlateVisibility::Collapsed);
	PendingSlotIndex = INDEX_NONE;
}
