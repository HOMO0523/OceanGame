#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanSurvivalComponent.generated.h"

UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanSurvivalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanSurvivalComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void ApplySurvivalDelta(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void SetStats(float NewStamina, float NewHydration, float NewSatiety);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void ApplyRecovery(float StaminaDelta, float HydrationDelta, float SatietyDelta);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void SetHydrationDrainPerSecond(float NewHydrationDrainPerSecond);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void SetSatietyDrainPerSecond(float NewSatietyDrainPerSecond);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void SetStaminaRecoverPerSecond(float NewStaminaRecoverPerSecond);

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	float GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	float GetHydration() const { return Hydration; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	float GetSatiety() const { return Satiety; }

private:
	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Hydration = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Satiety = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0"))
	float HydrationDrainPerSecond = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0"))
	float SatietyDrainPerSecond = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0"))
	float StaminaRecoverPerSecond = 2.0f;
};
