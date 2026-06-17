#include "OceanPrototype/OceanSurvivalComponent.h"
#include "Ocean.h"

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
	if (bHasDied) return;

	const float dt = FMath::Max(0.0f, DeltaSeconds);

	// Normal passive changes
	Stamina   = FMath::Clamp(Stamina + StaminaRecoverPerSecond * dt, 0.0f, MaxStamina);
	Hydration = FMath::Clamp(Hydration - HydrationDrainPerSecond * dt, 0.0f, 100.0f);
	Satiety   = FMath::Clamp(Satiety - SatietyDrainPerSecond * dt, 0.0f, 100.0f);

	// Starvation damage: Hydration OR Satiety = 0 → drain Stamina (blood)
	const bool bStarved = (Hydration <= 0.0f) || (Satiety <= 0.0f);
	if (bStarved)
	{
		const float Drain = StaminaDrainPerSecondWhenStarved * dt;
		if (Drain > 0.0f)
		{
			UE_LOG(LogOcean, Log, TEXT("[TDD] Survival: starved drain sta=%.1f→%.1f hyd=%.0f food=%.0f"),
				Stamina, FMath::Clamp(Stamina - Drain, 0.0f, MaxStamina), Hydration, Satiety);
		}
		ApplyDamage(Drain);
	}

	// Full sustenance recovery: Hydration AND Satiety = 100 → recover Stamina
	const bool bFull = (Hydration >= 100.0f) && (Satiety >= 100.0f);
	if (bFull && !bStarved)
	{
		ApplyHeal(StaminaRecoveryPerSecondWhenFull * dt);
	}

	// Death check
	if (Stamina <= 0.0f && !bHasDied)
	{
		bHasDied = true;
		UE_LOG(LogOcean, Log, TEXT("[TDD] Survival: DEATH stamina=0"));
		OnDeath.Broadcast();
	}
}

void UOceanSurvivalComponent::SetStats(float NewStamina, float NewHydration, float NewSatiety)
{
	Stamina   = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
	Hydration = FMath::Clamp(NewHydration, 0.0f, 100.0f);
	Satiety   = FMath::Clamp(NewSatiety, 0.0f, 100.0f);
}

void UOceanSurvivalComponent::ApplyRecovery(float StaminaDelta, float HydrationDelta, float SatietyDelta, float HealthDelta)
{
	if (bHasDied) return;

	Stamina   = FMath::Clamp(Stamina + StaminaDelta, 0.0f, MaxStamina);
	Hydration = FMath::Clamp(Hydration + HydrationDelta, 0.0f, 100.0f);
	Satiety   = FMath::Clamp(Satiety + SatietyDelta, 0.0f, 100.0f);

	// HealthDelta forwards to stamina (stamina IS the blood bar)
	if (HealthDelta != 0.0f)
	{
		if (HealthDelta > 0.0f) ApplyHeal(HealthDelta);
		else ApplyDamage(-HealthDelta);
	}
}

void UOceanSurvivalComponent::ApplyDamage(float Amount)
{
	if (bHasDied || Amount <= 0.0f) return;

	const float Old = Stamina;
	Stamina = FMath::Clamp(Stamina - Amount, 0.0f, MaxStamina);

	UE_LOG(LogOcean, Log, TEXT("[TDD] Survival: damage sta=%.1f→%.1f dmg=%.1f"), Old, Stamina, Amount);

	if (OnStaminaChanged.IsBound()) OnStaminaChanged.Broadcast(Stamina);

	if (Stamina <= 0.0f && !bHasDied)
	{
		bHasDied = true;
		UE_LOG(LogOcean, Log, TEXT("[TDD] Survival: DEATH stamina=0"));
		OnDeath.Broadcast();
	}
}

void UOceanSurvivalComponent::ApplyHeal(float Amount)
{
	if (bHasDied || Amount <= 0.0f) return;

	const float Old = Stamina;
	Stamina = FMath::Clamp(Stamina + Amount, 0.0f, MaxStamina);

	if (Stamina != Old)
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] Survival: heal sta=%.1f→%.1f heal=%.1f"), Old, Stamina, Amount);
		if (OnStaminaChanged.IsBound()) OnStaminaChanged.Broadcast(Stamina);
	}
}

void UOceanSurvivalComponent::SetHydrationDrainPerSecond(float v) { HydrationDrainPerSecond = FMath::Max(0.0f, v); }
void UOceanSurvivalComponent::SetSatietyDrainPerSecond(float v) { SatietyDrainPerSecond = FMath::Max(0.0f, v); }
void UOceanSurvivalComponent::SetStaminaRecoverPerSecond(float v) { StaminaRecoverPerSecond = FMath::Max(0.0f, v); }
