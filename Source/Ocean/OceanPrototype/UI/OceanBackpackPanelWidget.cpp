#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "Ocean.h"

void UOceanBackpackPanelWidget::BindInventory(UOceanInventoryComponent* InInventory, UOceanSurvivalComponent* InSurvival)
{
	InventoryComponent = InInventory;
	SurvivalComponent = InSurvival;

	if (InventoryComponent)
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: bound=1 slot_count=%d max_slots=%d"),
			InventoryComponent->GetSlots().Num(),
			GetMaxSlots());
	}
	else
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanBackpackPanel: bound=0 (null inventory)"));
	}

	RefreshSlots();
}

void UOceanBackpackPanelWidget::RefreshSlots()
{
	if (InventoryComponent)
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: refresh slots=%d"), InventoryComponent->GetSlots().Num());
	}
	OnInventoryRebuilt();
}

const TArray<FOceanInventorySlot>& UOceanBackpackPanelWidget::GetSlots() const
{
	if (InventoryComponent)
	{
		return InventoryComponent->GetSlots();
	}

	static const TArray<FOceanInventorySlot> EmptySlots;
	return EmptySlots;
}

int32 UOceanBackpackPanelWidget::GetMaxSlots() const
{
	if (InventoryComponent)
	{
		return InventoryComponent->GetSlots().Num() > 0 ? InventoryComponent->GetSlots().Num() : 12;
	}
	return 0;
}

bool UOceanBackpackPanelWidget::TryUseItemAtSlot(int32 SlotIndex)
{
	if (!InventoryComponent || !SurvivalComponent)
	{
		OnItemUseFailed(FText::FromString(TEXT("背包或生存组件未绑定")));
		return false;
	}

	const bool bSuccess = InventoryComponent->TryUseItemAtSlot(SlotIndex, SurvivalComponent);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: use_slot=%d result=%s"),
		SlotIndex, bSuccess ? TEXT("PASS") : TEXT("FAIL"));

	if (bSuccess)
	{
		RefreshSlots();
	}
	else
	{
		OnItemUseFailed(FText::FromString(TEXT("无法使用此物品")));
	}

	return bSuccess;
}

EOceanInventoryDragDropResult UOceanBackpackPanelWidget::HandleSlotDrop(int32 FromSlotIndex, int32 ToSlotIndex)
{
	if (!InventoryComponent)
	{
		return EOceanInventoryDragDropResult::Rejected;
	}

	const EOceanInventoryDragDropResult Result = InventoryComponent->MoveOrMergeSlot(FromSlotIndex, ToSlotIndex);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: drop from=%d to=%d result=%d"),
		FromSlotIndex, ToSlotIndex, static_cast<int32>(Result));

	if (Result != EOceanInventoryDragDropResult::Rejected)
	{
		RefreshSlots();
	}

	return Result;
}

void UOceanBackpackPanelWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (!InventoryComponent)
	{
		return;
	}

	// 检测槽位数量变化，自动刷新
	const int32 CurrentSlotCount = InventoryComponent->GetSlots().Num();
	if (CurrentSlotCount != LastCachedSlotCount)
	{
		LastCachedSlotCount = CurrentSlotCount;
		RefreshSlots();
	}
}
