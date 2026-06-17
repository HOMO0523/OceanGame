#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanSurvivalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedSignature, float, NewStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

/**
 * Survival component tracking Stamina (3-cell blood bar), Hydration, and Satiety.
 * Per ocean.docx: Stamina IS the blood/HP bar (3 cells). Death = Stamina <= 0.
 * Hydration/Satiety at 0 → drains Stamina. Hydration+Satiety full → recovers Stamina.
 */
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
	void ApplyRecovery(float StaminaDelta, float HydrationDelta, float SatietyDelta, float HealthDelta = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void SetHydrationDrainPerSecond(float NewHydrationDrainPerSecond);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void SetSatietyDrainPerSecond(float NewSatietyDrainPerSecond);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void SetStaminaRecoverPerSecond(float NewStaminaRecoverPerSecond);

	// Stamina = blood bar (3 cells). Damage reduces stamina. Heal restores it.
	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void ApplyDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
	void ApplyHeal(float Amount);

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	float GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	float GetHydration() const { return Hydration; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	float GetSatiety() const { return Satiety; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	float GetHealth() const { return Stamina; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	float GetMaxHealth() const { return MaxStamina; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Survival")
	bool IsDead() const { return bHasDied || Stamina <= 0.0f; }

	UPROPERTY(BlueprintAssignable, Category = "Ocean|Survival")
	FOnStaminaChangedSignature OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ocean|Survival")
	FOnDeathSignature OnDeath;

private:
	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Hydration = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Satiety = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "1.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0"))
	float HydrationDrainPerSecond = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0"))
	float SatietyDrainPerSecond = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0"))
	float StaminaRecoverPerSecond = 2.0f;

	// When Hydration OR Satiety = 0, drain Stamina (blood) per second
	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0"))
	float StaminaDrainPerSecondWhenStarved = 30.0f;

	// When Hydration AND Satiety = 100, recover Stamina (blood) per second
	UPROPERTY(EditAnywhere, Category = "Ocean|Survival", meta = (ClampMin = "0.0"))
	float StaminaRecoveryPerSecondWhenFull = 20.0f;

	UPROPERTY(VisibleAnywhere, Category = "Ocean|Survival")
	bool bHasDied = false;
};
