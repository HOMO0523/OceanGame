#include "OceanPrototype/OceanSurvivalComponent.h"

UOceanSurvivalComponent::UOceanSurvivalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UOceanSurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplySurvivalDelta(DeltaTime);
}

void UOceanSurvivalComponent::ApplySurvivalDelta(float DeltaSeconds)
{
	const float ClampedDeltaSeconds = FMath::Max(0.0f, DeltaSeconds);
	Stamina = FMath::Clamp(Stamina + StaminaRecoverPerSecond * ClampedDeltaSeconds, 0.0f, 100.0f);
	Hydration = FMath::Clamp(Hydration - HydrationDrainPerSecond * ClampedDeltaSeconds, 0.0f, 100.0f);
	Satiety = FMath::Clamp(Satiety - SatietyDrainPerSecond * ClampedDeltaSeconds, 0.0f, 100.0f);
}

void UOceanSurvivalComponent::SetStats(float NewStamina, float NewHydration, float NewSatiety)
{
	Stamina = FMath::Clamp(NewStamina, 0.0f, 100.0f);
	Hydration = FMath::Clamp(NewHydration, 0.0f, 100.0f);
	Satiety = FMath::Clamp(NewSatiety, 0.0f, 100.0f);
}

void UOceanSurvivalComponent::ApplyRecovery(float StaminaDelta, float HydrationDelta, float SatietyDelta)
{
	Stamina = FMath::Clamp(Stamina + StaminaDelta, 0.0f, 100.0f);
	Hydration = FMath::Clamp(Hydration + HydrationDelta, 0.0f, 100.0f);
	Satiety = FMath::Clamp(Satiety + SatietyDelta, 0.0f, 100.0f);
}

void UOceanSurvivalComponent::SetHydrationDrainPerSecond(float NewHydrationDrainPerSecond)
{
	HydrationDrainPerSecond = FMath::Max(0.0f, NewHydrationDrainPerSecond);
}

void UOceanSurvivalComponent::SetSatietyDrainPerSecond(float NewSatietyDrainPerSecond)
{
	SatietyDrainPerSecond = FMath::Max(0.0f, NewSatietyDrainPerSecond);
}

void UOceanSurvivalComponent::SetStaminaRecoverPerSecond(float NewStaminaRecoverPerSecond)
{
	StaminaRecoverPerSecond = FMath::Max(0.0f, NewStaminaRecoverPerSecond);
}
