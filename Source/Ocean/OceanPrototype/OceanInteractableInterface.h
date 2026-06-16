#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OceanInteractableInterface.generated.h"

UINTERFACE(BlueprintType)
class OCEAN_API UOceanInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class OCEAN_API IOceanInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ocean|Interaction")
	FText GetOceanInteractionText(AActor* Interactor) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ocean|Interaction")
	bool CanOceanInteract(AActor* Interactor, FText& FailureReason) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ocean|Interaction")
	bool ExecuteOceanInteraction(AActor* Interactor, FText& OutMessage);
};
