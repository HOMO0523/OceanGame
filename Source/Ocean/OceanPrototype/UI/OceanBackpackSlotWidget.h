#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanBackpackSlotWidget.generated.h"

class UBorder;
class UTextBlock;
class UVerticalBox;
class USizeBox;

UCLASS()
class OCEAN_API UOceanBackpackSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSlotData(int32 InSlotIndex, const FOceanInventorySlot& InSlot);
	void ClearSlot();

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Backpack")
	int32 GetSlotIndex() const { return SlotIndex; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Backpack")
	const FOceanItemStack& GetCachedStack() const { return CachedStack; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Backpack")
	bool IsOccupied() const { return bOccupied; }

	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Backpack")
	bool RequestUse();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

private:
	int32 SlotIndex = INDEX_NONE;
	FOceanItemStack CachedStack;
	bool bOccupied = false;
	bool bIsInitialized = false;

	// Visual elements
	UPROPERTY() TObjectPtr<USizeBox> RootSizeBox;
	UPROPERTY() TObjectPtr<UBorder> SlotBorder;
	UPROPERTY() TObjectPtr<UTextBlock> ItemNameText;
	UPROPERTY() TObjectPtr<UTextBlock> QuantityText;

	void UpdateVisuals();
};
