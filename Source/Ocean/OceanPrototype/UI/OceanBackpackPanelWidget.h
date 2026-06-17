#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanBackpackPanelWidget.generated.h"

class UOceanInventoryComponent;
class UOceanSurvivalComponent;
class UOceanBackpackSlotWidget;

UCLASS()
class OCEAN_API UOceanBackpackPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 绑定背包组件。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Backpack")
	void BindInventory(UOceanInventoryComponent* InInventory, UOceanSurvivalComponent* InSurvival);

	/** 手动刷新槽位列表。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Backpack")
	void RefreshSlots();

	/** 获取槽位数据（供 WBP 创建子控件使用）。 */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Backpack")
	const TArray<FOceanInventorySlot>& GetSlots() const;

	/** 获取最大槽位数。 */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Backpack")
	int32 GetMaxSlots() const;

	/** 使用指定槽位的物品。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Backpack")
	bool TryUseItemAtSlot(int32 SlotIndex);

	/** 拖拽放置处理（源槽位→目标槽位）。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Backpack")
	EOceanInventoryDragDropResult HandleSlotDrop(int32 FromSlotIndex, int32 ToSlotIndex);

protected:
	/** WBP 实现：刷新所有子控件。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Backpack")
	void OnInventoryRebuilt();

	/** WBP 实现：物品使用失败提示。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Backpack")
	void OnItemUseFailed(const FText& Reason);

private:
	/** 弱引用，避免阻挡 GC。 */
	UPROPERTY(Transient)
	TObjectPtr<UOceanInventoryComponent> InventoryComponent;

	/** 弱引用生存组件。 */
	UPROPERTY(Transient)
	TObjectPtr<UOceanSurvivalComponent> SurvivalComponent;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	int32 LastCachedSlotCount = -1;
};
