#include "OceanPrototype/UI/OceanBackpackSlotWidget.h"
#include "Ocean.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UOceanBackpackSlotWidget::SetSlotData(int32 InSlotIndex, const FOceanInventorySlot& InSlot)
{
	SlotIndex = InSlotIndex;
	CachedStack = InSlot.Stack;
	bOccupied = InSlot.Stack.Quantity > 0 && !InSlot.Stack.ItemId.IsNone();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackSlot: set slot=%d item=%s qty=%d"), SlotIndex, *CachedStack.ItemId.ToString(), CachedStack.Quantity);

	if (bOccupied)
	{
		OnSlotDataUpdated(true, FText::FromName(CachedStack.ItemId), CachedStack.Quantity, CachedStack.MaxStack, CachedStack.Category);
	}
	else
	{
		OnSlotCleared();
	}
}

void UOceanBackpackSlotWidget::ClearSlot()
{
	SlotIndex = INDEX_NONE;
	CachedStack = FOceanItemStack();
	bOccupied = false;
	OnSlotCleared();
}

FReply UOceanBackpackSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bOccupied)
	{
		FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		return Reply.NativeReply;
	}
	return FReply::Handled();
}

void UOceanBackpackSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (!bOccupied)
	{
		return;
	}

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
