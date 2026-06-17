#include "OceanPrototype/OceanItemDefinition.h"

UOceanItemDefinition::UOceanItemDefinition()
{
	// Defaults intentionally minimal; designers override in the DataAsset instance.
}

FOceanItemStack UOceanItemDefinition::ToItemStack(int32 Quantity) const
{
	FOceanItemStack Stack;
	Stack.ItemId = ItemId;
	Stack.Quantity = FMath::Max(0, Quantity);
	Stack.MaxStack = FMath::Max(1, MaxStack);
	Stack.Category = Category;
	Stack.UseEffect = UseEffect;
	Stack.bKeyItem = bKeyItem;
	Stack.bPlaceable = bPlaceable;
	return Stack;
}

FPrimaryAssetId UOceanItemDefinition::GetPrimaryAssetId() const
{
	// Bucket all item definitions under the "OceanItem" asset type so the asset
	// manager can scan and load them as a group. Use ItemId when set, otherwise
	// fall back to the asset's package name.
	const FName AssetType = TEXT("OceanItem");
	FName PrimaryId = ItemId;
	if (PrimaryId.IsNone())
	{
		PrimaryId = FName(*GetPathName());
	}
	return FPrimaryAssetId(AssetType, PrimaryId);
}
