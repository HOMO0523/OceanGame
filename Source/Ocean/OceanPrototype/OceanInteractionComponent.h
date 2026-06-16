#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanInteractionComponent.generated.h"

UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Interaction")
	AActor* FindBestInteractable() const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Interaction")
	bool TryInteract(FText& OutMessage);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Interaction")
	void SetInteractionRadius(float NewRadius);

private:
	UPROPERTY(EditAnywhere, Category = "Ocean|Interaction", meta = (ClampMin = "1.0"))
	float InteractionRadius = 250.0f;
};
