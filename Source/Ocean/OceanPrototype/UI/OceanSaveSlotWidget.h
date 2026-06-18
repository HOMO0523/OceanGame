#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanSaveSlotWidget.generated.h"

class UButton;
class UTextBlock;
class UHorizontalBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotLoadClicked, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotDeleteClicked, int32, SlotIndex);

/**
 * A single save slot row in the main menu / save menu.
 * Shows: Slot N, Day X, Timestamp, [Load] [Delete]
 */
UCLASS()
class OCEAN_API UOceanSaveSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Static: which slot to auto-load on level start (0 = none)
	static int32 PendingLoadSlot;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Ocean|SaveSlot")
	void SetSlotIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Ocean|SaveSlot")
	void RefreshInfo();

	UPROPERTY(BlueprintAssignable, Category = "Ocean|SaveSlot")
	FOnSlotLoadClicked OnLoadClicked;

	UPROPERTY(BlueprintAssignable, Category = "Ocean|SaveSlot")
	FOnSlotDeleteClicked OnDeleteClicked;

protected:
	UPROPERTY() TObjectPtr<UTextBlock> SlotLabel;
	UPROPERTY() TObjectPtr<UButton> LoadButton;
	UPROPERTY() TObjectPtr<UButton> DeleteButton;

	int32 SlotIndex = 1;
	bool bIsInitialized = false;

	UFUNCTION() void OnLoadButtonClicked();
	UFUNCTION() void OnDeleteButtonClicked();
};
