#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"

UOceanInventoryComponent::UOceanInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UOceanInventoryComponent::AddResource(FOceanResourceStack Stack)
{
	if (Stack.Amount <= 0)
	{
		return false;
	}

	if (FOceanResourceStack* ExistingStack = FindMutableStack(Stack.ResourceType))
	{
		ExistingStack->Amount += Stack.Amount;
		return true;
	}

	if (Stacks.Num() >= MaxSlots)
	{
		return false;
	}

	Stacks.Add(Stack);
	return true;
}

bool UOceanInventoryComponent::AddItem(const FOceanItemStack& Stack)
{
	if (Stack.ItemId.IsNone() || Stack.Quantity <= 0 || Stack.MaxStack <= 0)
	{
		return false;
	}

	int32 Remaining = Stack.Quantity;
	for (const FOceanInventorySlot& Slot : Slots)
	{
		if (Slot.Stack.ItemId == Stack.ItemId && Slot.Stack.Quantity < Slot.Stack.MaxStack)
		{
			const int32 Capacity = Slot.Stack.MaxStack - Slot.Stack.Quantity;
			Remaining -= FMath::Min(Capacity, Remaining);
			if (Remaining <= 0)
			{
				break;
			}
		}
	}

	if (Remaining > 0)
	{
		const int32 RequiredNewSlots = FMath::DivideAndRoundUp(Remaining, Stack.MaxStack);
		if (Slots.Num() + RequiredNewSlots > MaxItemSlots)
		{
			return false;
		}
	}

	Remaining = Stack.Quantity;
	for (FOceanInventorySlot& Slot : Slots)
	{
		if (Slot.Stack.ItemId == Stack.ItemId && Slot.Stack.Quantity < Slot.Stack.MaxStack)
		{
			const int32 Capacity = Slot.Stack.MaxStack - Slot.Stack.Quantity;
			const int32 ToMove = FMath::Min(Capacity, Remaining);
			Slot.Stack.Quantity += ToMove;
			Remaining -= ToMove;
			if (Remaining <= 0)
			{
				return true;
			}
		}
	}

	while (Remaining > 0)
	{
		FOceanInventorySlot NewSlot;
		NewSlot.SlotIndex = Slots.Num();
		NewSlot.Stack = Stack;
		NewSlot.Stack.Quantity = FMath::Min(Stack.MaxStack, Remaining);
		Slots.Add(NewSlot);
		Remaining -= NewSlot.Stack.Quantity;
	}

	return true;
}

bool UOceanInventoryComponent::TryUseItemAtSlot(int32 SlotIndex, UOceanSurvivalComponent* Survival)
{
	if (!Slots.IsValidIndex(SlotIndex) || !IsValid(Survival))
	{
		return false;
	}

	FOceanInventorySlot& Slot = Slots[SlotIndex];
	if (Slot.Stack.Category != EOceanItemCategory::Consumable || Slot.Stack.Quantity <= 0)
	{
		return false;
	}

	Survival->ApplyRecovery(
		Slot.Stack.UseEffect.StaminaDelta,
		Slot.Stack.UseEffect.HydrationDelta,
		Slot.Stack.UseEffect.SatietyDelta,
		Slot.Stack.UseEffect.HealthDelta);

	Slot.Stack.Quantity -= 1;
	if (Slot.Stack.Quantity <= 0)
	{
		Slots.RemoveAt(SlotIndex);
		for (int32 Index = 0; Index < Slots.Num(); ++Index)
		{
			Slots[Index].SlotIndex = Index;
		}
	}

	return true;
}

EOceanInventoryDragDropResult UOceanInventoryComponent::MoveOrMergeSlot(int32 FromSlotIndex, int32 ToSlotIndex)
{
	if (!Slots.IsValidIndex(FromSlotIndex) || !Slots.IsValidIndex(ToSlotIndex) || FromSlotIndex == ToSlotIndex)
	{
		return EOceanInventoryDragDropResult::Rejected;
	}

	FOceanInventorySlot& FromSlot = Slots[FromSlotIndex];
	FOceanInventorySlot& ToSlot = Slots[ToSlotIndex];

	if (FromSlot.bLocked || ToSlot.bLocked)
	{
		return EOceanInventoryDragDropResult::Rejected;
	}

	if (FromSlot.Stack.ItemId == ToSlot.Stack.ItemId)
	{
		const int32 Capacity = ToSlot.Stack.MaxStack - ToSlot.Stack.Quantity;
		if (Capacity <= 0)
		{
			return EOceanInventoryDragDropResult::Rejected;
		}

		const int32 ToMove = FMath::Min(Capacity, FromSlot.Stack.Quantity);
		ToSlot.Stack.Quantity += ToMove;
		FromSlot.Stack.Quantity -= ToMove;
		if (FromSlot.Stack.Quantity <= 0)
		{
			Slots.RemoveAt(FromSlotIndex);
		}

		for (int32 Index = 0; Index < Slots.Num(); ++Index)
		{
			Slots[Index].SlotIndex = Index;
		}
		return EOceanInventoryDragDropResult::Merged;
	}

	Swap(FromSlot.Stack, ToSlot.Stack);
	return EOceanInventoryDragDropResult::Swapped;
}

bool UOceanInventoryComponent::CanAcceptResource(FOceanResourceStack Stack) const
{
	if (Stack.Amount <= 0)
	{
		return false;
	}

	if (FindStack(Stack.ResourceType))
	{
		return true;
	}

	return Stacks.Num() < MaxSlots;
}

bool UOceanInventoryComponent::CanAfford(const TArray<FOceanResourceStack>& Cost) const
{
	TMap<EOceanResourceType, int32> AggregatedCost;

	for (const FOceanResourceStack& CostStack : Cost)
	{
		if (CostStack.Amount <= 0)
		{
			return false;
		}

		AggregatedCost.FindOrAdd(CostStack.ResourceType) += CostStack.Amount;
	}

	for (const TPair<EOceanResourceType, int32>& CostEntry : AggregatedCost)
	{
		if (GetResourceAmount(CostEntry.Key) < CostEntry.Value)
		{
			return false;
		}
	}

	return true;
}

bool UOceanInventoryComponent::TrySpend(const TArray<FOceanResourceStack>& Cost)
{
	if (!CanAfford(Cost))
	{
		return false;
	}

	TMap<EOceanResourceType, int32> AggregatedCost;
	for (const FOceanResourceStack& CostStack : Cost)
	{
		AggregatedCost.FindOrAdd(CostStack.ResourceType) += CostStack.Amount;
	}

	for (const TPair<EOceanResourceType, int32>& CostEntry : AggregatedCost)
	{
		if (FOceanResourceStack* ExistingStack = FindMutableStack(CostEntry.Key))
		{
			ExistingStack->Amount -= CostEntry.Value;
		}
	}

	Stacks.RemoveAll([](const FOceanResourceStack& Stack)
	{
		return Stack.Amount <= 0;
	});

	return true;
}

int32 UOceanInventoryComponent::GetResourceAmount(EOceanResourceType ResourceType) const
{
	if (const FOceanResourceStack* ExistingStack = FindStack(ResourceType))
	{
		return ExistingStack->Amount;
	}

	return 0;
}

void UOceanInventoryComponent::SetMaxSlots(int32 NewMaxSlots)
{
	MaxSlots = FMath::Max(1, NewMaxSlots);
}

FOceanResourceStack* UOceanInventoryComponent::FindMutableStack(EOceanResourceType ResourceType)
{
	return Stacks.FindByPredicate([ResourceType](const FOceanResourceStack& Stack)
	{
		return Stack.ResourceType == ResourceType;
	});
}

const FOceanResourceStack* UOceanInventoryComponent::FindStack(EOceanResourceType ResourceType) const
{
	return Stacks.FindByPredicate([ResourceType](const FOceanResourceStack& Stack)
	{
		return Stack.ResourceType == ResourceType;
	});
}

bool UOceanInventoryComponent::HasItem(FName ItemId) const
{
	for (const FOceanInventorySlot& Slot : Slots)
	{
		if (Slot.Stack.ItemId == ItemId && Slot.Stack.Quantity > 0)
		{
			return true;
		}
	}
	return false;
}
