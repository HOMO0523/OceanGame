#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanBackpackPanelWidget.generated.h"

class UOceanInventoryComponent;
class UOceanSurvivalComponent;
class UTextBlock;
class UVerticalBox;

UCLASS()
class OCEAN_API UOceanBackpackPanelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void BindInventory(UOceanInventoryComponent* InInventory, UOceanSurvivalComponent* InSurvival);
	void RefreshSlots();
	const TArray<FOceanInventorySlot>& GetSlots() const;
	int32 GetMaxSlots() const;
	bool TryUseItemAtSlot(int32 SlotIndex);
	EOceanInventoryDragDropResult HandleSlotDrop(int32 FromSlotIndex, int32 ToSlotIndex);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY(Transient) TObjectPtr<UOceanInventoryComponent> InventoryComponent;
	UPROPERTY(Transient) TObjectPtr<UOceanSurvivalComponent> SurvivalComponent;
	int32 LastCachedSlotCount = -1;

	UPROPERTY() TObjectPtr<UVerticalBox> SlotContainer;
	UPROPERTY() TObjectPtr<UTextBlock> HeaderText;
	UPROPERTY() TObjectPtr<UTextBlock> SlotCountText;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
};
