#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OceanResourceField.generated.h"

class AOceanResourceNode;
class UPCGComponent;
class USceneComponent;

UCLASS(Blueprintable)
class OCEAN_API AOceanResourceField : public AActor
{
	GENERATED_BODY()

public:
	AOceanResourceField();

	UFUNCTION(BlueprintPure, Category = "Ocean|Resources")
	UPCGComponent* GetPCGComponent() const { return PCGComponent; }

	UFUNCTION(BlueprintCallable, Category = "Ocean|Resources")
	TArray<FVector> GenerateFallbackResourceLocations(FVector Center) const;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Resources")
	void SpawnFallbackResources(FVector Center);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Resources")
	void SetFallbackResourceCount(int32 NewCount);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Resources")
	void SetFieldRadius(float NewRadius);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Resources")
	void SetExclusionRadius(float NewRadius);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Resources")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Resources")
	TObjectPtr<UPCGComponent> PCGComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Resources")
	TSubclassOf<AOceanResourceNode> ResourceNodeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Resources", meta = (ClampMin = "0"))
	int32 FallbackResourceCount = 24;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Resources", meta = (ClampMin = "1.0"))
	float FieldRadius = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Resources", meta = (ClampMin = "0.0"))
	float ExclusionRadius = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ocean|Resources")
	int32 Seed = 1337;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean|Resources")
	TArray<TObjectPtr<AOceanResourceNode>> SpawnedResources;
};
