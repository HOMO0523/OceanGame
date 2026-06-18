#include "OceanPrototype/UI/OceanBackpackSlotWidget.h"
#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "Ocean.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/SizeBox.h"

TSharedRef<SWidget> UOceanBackpackSlotWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	// Fixed-size slot: 80x80
	RootSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("RootSizeBox"));
	RootSizeBox->SetWidthOverride(80.0f);
	RootSizeBox->SetHeightOverride(80.0f);
	WidgetTree->RootWidget = RootSizeBox;

	// Border with dark background
	SlotBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("SlotBorder"));
	SlotBorder->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.12f, 0.9f));
	SlotBorder->SetPadding(FMargin(4));
	RootSizeBox->AddChild(SlotBorder);

	// Vertical box: item name + quantity
	auto* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	SlotBorder->AddChild(VBox);

	ItemNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ItemNameText"));
	ItemNameText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
	ItemNameText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 9));
	ItemNameText->SetJustification(ETextJustify::Center);
	ItemNameText->SetAutoWrapText(true);
	VBox->AddChildToVerticalBox(ItemNameText);

	QuantityText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("QuantityText"));
	QuantityText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	QuantityText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16));
	QuantityText->SetJustification(ETextJustify::Center);
	VBox->AddChildToVerticalBox(QuantityText);

	UpdateVisuals();
	return Super::RebuildWidget();
}

void UOceanBackpackSlotWidget::SetSlotData(int32 InSlotIndex, const FOceanInventorySlot& InSlot)
{
	SlotIndex = InSlotIndex;
	CachedStack = InSlot.Stack;
	bOccupied = InSlot.Stack.Quantity > 0 && !InSlot.Stack.ItemId.IsNone();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackSlot: set slot=%d item=%s qty=%d occupied=%d"),
		SlotIndex, *CachedStack.ItemId.ToString(), CachedStack.Quantity, bOccupied ? 1 : 0);

	UpdateVisuals();
}

void UOceanBackpackSlotWidget::ClearSlot()
{
	SlotIndex = INDEX_NONE;
	CachedStack = FOceanItemStack();
	bOccupied = false;
	UpdateVisuals();
}

void UOceanBackpackSlotWidget::UpdateVisuals()
{
	if (!SlotBorder) return;

	if (bOccupied)
	{
		// Filled slot: bright border
		SlotBorder->SetBrushColor(FLinearColor(0.12f, 0.2f, 0.08f, 0.95f));

		if (ItemNameText)
		{
			ItemNameText->SetText(FText::FromName(CachedStack.ItemId));
		}
		if (QuantityText)
		{
			QuantityText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), CachedStack.Quantity)));
		}
	}
	else
	{
		// Empty slot: dark gray
		SlotBorder->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.08f, 0.7f));
		if (ItemNameText) ItemNameText->SetText(FText::GetEmpty());
		if (QuantityText) QuantityText->SetText(FText::GetEmpty());
	}
}

bool UOceanBackpackSlotWidget::RequestUse()
{
	const int32 RequestedSlotIndex = SlotIndex;
	const FName RequestedItemId = CachedStack.ItemId;

	if (!bOccupied || RequestedSlotIndex == INDEX_NONE)
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackSlot: use_rejected slot=%d occupied=%d"), RequestedSlotIndex, bOccupied ? 1 : 0);
		return false;
	}

	UOceanBackpackPanelWidget* OwningPanel = GetTypedOuter<UOceanBackpackPanelWidget>();
	if (!OwningPanel)
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanBackpackSlot: use_rejected slot=%d reason=no_panel"), RequestedSlotIndex);
		return false;
	}

	const bool bUsed = OwningPanel->TryUseItemAtSlot(RequestedSlotIndex);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackSlot: request_use slot=%d item=%s result=%s"),
		RequestedSlotIndex,
		*RequestedItemId.ToString(),
		bUsed ? TEXT("PASS") : TEXT("FAIL"));
	return bUsed;
}

FReply UOceanBackpackSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bOccupied)
	{
		FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		return Reply.NativeReply;
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && bOccupied)
	{
		RequestUse();
		return FReply::Handled();
	}

	return FReply::Handled();
}

void UOceanBackpackSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (!bOccupied) return;
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UDragDropOperation* DragOp = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	if (DragOp)
	{
		DragOp->Payload = this;
		DragOp->DefaultDragVisual = this;
		DragOp->Pivot = EDragPivot::CenterCenter;
		OutOperation = DragOp;
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackSlot: drag_started slot=%d item=%s"), SlotIndex, *CachedStack.ItemId.ToString());
	}
}
