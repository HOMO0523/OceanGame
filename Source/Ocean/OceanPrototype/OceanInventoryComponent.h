#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanResourceTypes.h"
#include "OceanInventoryComponent.generated.h"

UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	bool AddResource(FOceanResourceStack Stack);

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	bool CanAfford(const TArray<FOceanResourceStack>& Cost) const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	bool TrySpend(const TArray<FOceanResourceStack>& Cost);

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	int32 GetResourceAmount(EOceanResourceType ResourceType) const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
	void SetMaxSlots(int32 NewMaxSlots);

	UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
	const TArray<FOceanResourceStack>& GetStacks() const { return Stacks; }

private:
	UPROPERTY(EditAnywhere, Category = "Ocean|Inventory", meta = (ClampMin = "1"))
	int32 MaxSlots = 8;

	UPROPERTY(VisibleAnywhere, Category = "Ocean|Inventory")
	TArray<FOceanResourceStack> Stacks;

	FOceanResourceStack* FindMutableStack(EOceanResourceType ResourceType);
	const FOceanResourceStack* FindStack(EOceanResourceType ResourceType) const;
};
