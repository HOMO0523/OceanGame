#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanItemUseModalWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUseConfirmedSignature, int32, SlotIndex);

UCLASS()
class OCEAN_API UOceanItemUseModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 弹出确认弹窗。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Modal")
	void ShowConfirmation(int32 InSlotIndex, const FOceanItemStack& InItem);

	/** 确认使用。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Modal")
	void Confirm();

	/** 取消。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Modal")
	void Reject();

	/** 用户确认使用回调。 */
	UPROPERTY(BlueprintAssignable, Category = "Ocean|UI|Modal")
	FOnItemUseConfirmedSignature OnItemUseConfirmed;

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Modal")
	int32 GetPendingSlotIndex() const { return PendingSlotIndex; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Modal")
	const FOceanItemStack& GetPendingItem() const { return PendingItem; }

protected:
	/** WBP 实现：弹窗显示/隐藏。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Modal")
	void OnModalStateChanged(bool bVisible);

	/** WBP 实现：物品信息更新。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Modal")
	void OnItemInfoUpdated(const FText& ItemDisplayName, EOceanItemCategory Category);

private:
	int32 PendingSlotIndex = INDEX_NONE;
	FOceanItemStack PendingItem;
};
