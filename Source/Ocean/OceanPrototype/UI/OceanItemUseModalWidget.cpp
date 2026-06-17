#include "OceanPrototype/UI/OceanItemUseModalWidget.h"
#include "Ocean.h"

void UOceanItemUseModalWidget::ShowConfirmation(int32 InSlotIndex, const FOceanItemStack& InItem)
{
	PendingSlotIndex = InSlotIndex;
	PendingItem = InItem;

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanItemUseModal: show slot=%d item=%s"), InSlotIndex, *InItem.ItemId.ToString());

	OnModalStateChanged(true);
	OnItemInfoUpdated(FText::FromName(InItem.ItemId), InItem.Category);
}

void UOceanItemUseModalWidget::Confirm()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanItemUseModal: confirmed slot=%d"), PendingSlotIndex);

	OnModalStateChanged(false);
	OnItemUseConfirmed.Broadcast(PendingSlotIndex);
	PendingSlotIndex = INDEX_NONE;
}

void UOceanItemUseModalWidget::Reject()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanItemUseModal: rejected slot=%d"), PendingSlotIndex);

	OnModalStateChanged(false);
	PendingSlotIndex = INDEX_NONE;
}
