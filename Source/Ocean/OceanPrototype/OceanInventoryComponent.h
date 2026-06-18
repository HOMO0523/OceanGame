#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanItemTypes.h"
#include "OceanResourceTypes.h"
#include "OceanInventoryComponent.generated.h"

class UOceanSurvivalComponent;

UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	bool AddResource(FOceanResourceStack Stack);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	bool AddItem(const FOceanItemStack& Stack);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	bool TryUseItemAtSlot(int32 SlotIndex, UOceanSurvivalComponent* Survival);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	EOceanInventoryDragDropResult MoveOrMergeSlot(int32 FromSlotIndex, int32 ToSlotIndex);

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	bool CanAcceptResource(FOceanResourceStack Stack) const;

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	bool CanAfford(const TArray<FOceanResourceStack>& Cost) const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	bool TrySpend(const TArray<FOceanResourceStack>& Cost);

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	int32 GetResourceAmount(EOceanResourceType ResourceType) const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	void SetMaxSlots(int32 NewMaxSlots);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	bool RestoreInventoryState(const TArray<FOceanResourceStack>& SavedStacks, const TArray<FOceanInventorySlot>& SavedSlots);

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	const TArray<FOceanResourceStack>& GetStacks() const { return Stacks; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	const TArray<FOceanInventorySlot>& GetSlots() const { return Slots; }

	/** Check if the inventory contains at least one item with the given ItemId. */
	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	bool HasItem(FName ItemId) const;

private:
	UPROPERTY(EditAnywhere, Category = "Ocean|Inventory", meta = (ClampMin = "1"))
	int32 MaxSlots = 8;

	UPROPERTY(VisibleAnywhere, Category = "Ocean|Inventory")
	TArray<FOceanResourceStack> Stacks;

	UPROPERTY(EditAnywhere, Category = "Ocean|Inventory", meta = (ClampMin = "1"))
	int32 MaxItemSlots = 12;

	UPROPERTY(VisibleAnywhere, Category = "Ocean|Inventory")
	TArray<FOceanInventorySlot> Slots;

	FOceanResourceStack* FindMutableStack(EOceanResourceType ResourceType);
	const FOceanResourceStack* FindStack(EOceanResourceType ResourceType) const;
};
