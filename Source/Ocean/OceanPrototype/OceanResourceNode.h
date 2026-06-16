#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OceanResourceTypes.h"
#include "OceanResourceNode.generated.h"

class UBuoyancyComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class OCEAN_API AOceanResourceNode : public AActor
{
	GENERATED_BODY()

public:
	AOceanResourceNode();

	UFUNCTION(BlueprintPure, Category = "Ocean|Resources")
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Water")
	UBuoyancyComponent* GetBuoyancyComponent() const { return BuoyancyComponent; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Resources")
	FOceanResourceStack GetResourceStack() const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Resources")
	FOceanResourceStack Collect();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Resources")
	void SetPlaceholderMesh(UStaticMesh* NewMesh);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Resources")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Resources")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Water")
	TObjectPtr<UBuoyancyComponent> BuoyancyComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Resources")
	EOceanResourceType ResourceType = EOceanResourceType::Wood;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Resources", meta = (ClampMin = "1"))
	int32 Amount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Resources", meta = (ClampMin = "1.0"))
	float CollectionRadius = 140.0f;
};
