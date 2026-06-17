#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanBackpackSlotWidget.generated.h"

class UOceanInventoryComponent;

UCLASS()
class OCEAN_API UOceanBackpackSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 设置此槽位的数据。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Backpack")
	void SetSlotData(int32 InSlotIndex, const FOceanInventorySlot& InSlot);

	/** 清空槽位。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Backpack")
	void ClearSlot();

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Backpack")
	int32 GetSlotIndex() const { return SlotIndex; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Backpack")
	const FOceanItemStack& GetCachedStack() const { return CachedStack; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Backpack")
	bool IsOccupied() const { return bOccupied; }

protected:
	/** WBP 实现：槽位数据变化时刷新视觉。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Backpack")
	void OnSlotDataUpdated(bool bNewOccupied, const FText& ItemDisplayName, int32 Quantity, int32 MaxStack, EOceanItemCategory Category);

	/** WBP 实现：槽位被清空时。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Backpack")
	void OnSlotCleared();

	/** 鼠标点击：通知面板尝试使用物品。 */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/** 拖拽检测：发起拖拽操作。 */
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

private:
	int32 SlotIndex = INDEX_NONE;
	FOceanItemStack CachedStack;
	bool bOccupied = false;
};
