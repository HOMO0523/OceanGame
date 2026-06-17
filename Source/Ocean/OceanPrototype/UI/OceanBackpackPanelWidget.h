#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanBackpackPanelWidget.generated.h"

class UOceanInventoryComponent;
class UOceanSurvivalComponent;
class UTextBlock;
class UVerticalBox;
class UBorder;
class UWrapBox;
class UOceanBackpackSlotWidget;

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
	bool bIsInitialized = false;

	static constexpr int32 SlotGridCount = 12;

	UPROPERTY() TObjectPtr<UBorder> BgBorder;
	UPROPERTY() TObjectPtr<UVerticalBox> RootVBox;
	UPROPERTY() TObjectPtr<UTextBlock> HeaderText;
	UPROPERTY() TObjectPtr<UTextBlock> SlotCountText;
	UPROPERTY() TObjectPtr<UWrapBox> SlotWrapBox;
	UPROPERTY() TArray<TObjectPtr<UOceanBackpackSlotWidget>> SlotWidgets;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
};
