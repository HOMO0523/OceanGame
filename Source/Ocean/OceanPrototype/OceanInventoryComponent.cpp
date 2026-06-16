#include "OceanPrototype/OceanInventoryComponent.h"

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
