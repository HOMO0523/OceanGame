#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OceanInteractableInterface.h"
#include "OceanItemPickupActor.generated.h"

class UOceanItemDefinition;
class UStaticMeshComponent;
class UTextRenderComponent;
class UBillboardComponent;
class USphereComponent;

/**
 * AOceanItemPickupActor
 * A world-space representation of a pickup-able item defined by a UOceanItemDefinition.
 * Uses a 30x30x30 cube placeholder mesh and a billboard / text label so the
 * placeholder is readable before any real art is wired up.
 *
 * Implements IOceanInteractableInterface so the existing interaction system
 * (player trace + interaction prompt) can pick it up without bespoke code.
 */
UCLASS(Blueprintable, BlueprintType)
class OCEAN_API AOceanItemPickupActor : public AActor, public IOceanInteractableInterface
{
	GENERATED_BODY()

public:
	AOceanItemPickupActor();

	/** Definition that drives this pickup's identity, stack rules and use effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	TObjectPtr<UOceanItemDefinition> ItemDefinition;

	/** Quantity granted on pickup. Clamped to [1, MaxStack] at pickup time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items", meta = (ClampMin = "1"))
	int32 Quantity = 1;

	/** Blueprint helper: configure this pickup from a definition at runtime. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|Items")
	void InitializeFromDefinition(UOceanItemDefinition* Definition, int32 InQuantity = 1);

	UFUNCTION(BlueprintPure, Category = "Ocean|Items")
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Items")
	UTextRenderComponent* GetLabelComponent() const { return LabelComponent; }

	//~ IOceanInteractableInterface begin
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ocean|Interaction")
	FText GetOceanInteractionText(AActor* Interactor) const;
	virtual FText GetOceanInteractionText_Implementation(AActor* Interactor) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ocean|Interaction")
	bool CanOceanInteract(AActor* Interactor, FText& FailureReason) const;
	virtual bool CanOceanInteract_Implementation(AActor* Interactor, FText& FailureReason) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ocean|Interaction")
	bool ExecuteOceanInteraction(AActor* Interactor, FText& OutMessage);
	virtual bool ExecuteOceanInteraction_Implementation(AActor* Interactor, FText& OutMessage) override;
	//~ IOceanInteractableInterface end

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Items")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Items")
	TObjectPtr<UTextRenderComponent> LabelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Items")
	TObjectPtr<UBillboardComponent> BillboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Items")
	TObjectPtr<USphereComponent> InteractionSphere;

private:
	/** Refresh the label text + scale from the current definition. */
	void RefreshVisuals();
};
