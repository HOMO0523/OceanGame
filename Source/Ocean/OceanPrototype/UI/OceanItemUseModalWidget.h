#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanItemTypes.h"
#include "OceanItemUseModalWidget.generated.h"

class UTextBlock;
class UBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUseConfirmedSignature, int32, SlotIndex);

UCLASS()
class OCEAN_API UOceanItemUseModalWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void ShowConfirmation(int32 InSlotIndex, const FOceanItemStack& InItem);
	void Confirm();
	void Reject();
	UPROPERTY(BlueprintAssignable) FOnItemUseConfirmedSignature OnItemUseConfirmed;
	int32 GetPendingSlotIndex() const { return PendingSlotIndex; }
	const FOceanItemStack& GetPendingItem() const { return PendingItem; }

protected:
	virtual void NativeConstruct() override;

private:
	int32 PendingSlotIndex = INDEX_NONE;
	FOceanItemStack PendingItem;
	UPROPERTY() TObjectPtr<UBorder> ModalBorder;
	UPROPERTY() TObjectPtr<UTextBlock> ItemNameText;
	UPROPERTY() TObjectPtr<UTextBlock> HintText;
};
