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

	// Health regulation based on starvation / fullness.
	if (!bHasDied)
	{
		const bool bStarved = (Hydration <= 0.0f) || (Satiety <= 0.0f);
		const bool bFullySustained = (Hydration >= 100.0f) && (Satiety >= 100.0f);

		if (bStarved)
		{
			const float DrainAmount = HealthDrainPerSecondWhenStarved * ClampedDeltaSeconds;
			if (DrainAmount > 0.0f)
			{
				UE_LOG(LogTemp, Log, TEXT("[TDD] Survival HealthDrain starved: Health=%f -> %f (drain=%f) Hydration=%f Satiety=%f"),
					Health, FMath::Clamp(Health - DrainAmount, 0.0f, MaxHealth), DrainAmount, Hydration, Satiety);
			}
			ApplyDamage(DrainAmount);
		}
		else if (bFullySustained)
		{
			const float RecoverAmount = HealthRecoveryPerSecondWhenFull * ClampedDeltaSeconds;
			if (RecoverAmount > 0.0f)
			{
				UE_LOG(LogTemp, Log, TEXT("[TDD] Survival HealthRecovery full: Health=%f -> %f (recover=%f) Hydration=%f Satiety=%f"),
					Health, FMath::Clamp(Health + RecoverAmount, 0.0f, MaxHealth), RecoverAmount, Hydration, Satiety);
			}
			ApplyHeal(RecoverAmount);
		}
	}
}

void UOceanSurvivalComponent::SetStats(float NewStamina, float NewHydration, float NewSatiety)
{
	Stamina = FMath::Clamp(NewStamina, 0.0f, 100.0f);
	Hydration = FMath::Clamp(NewHydration, 0.0f, 100.0f);
	Satiety = FMath::Clamp(NewSatiety, 0.0f, 100.0f);
}

void UOceanSurvivalComponent::ApplyRecovery(float StaminaDelta, float HydrationDelta, float SatietyDelta, float HealthDelta)
{
	Stamina = FMath::Clamp(Stamina + StaminaDelta, 0.0f, 100.0f);
	Hydration = FMath::Clamp(Hydration + HydrationDelta, 0.0f, 100.0f);
	Satiety = FMath::Clamp(Satiety + SatietyDelta, 0.0f, 100.0f);

	if (HealthDelta != 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("[TDD] Survival ApplyRecovery HealthDelta: Health=%f -> %f (delta=%f)"),
			Health, FMath::Clamp(Health + HealthDelta, 0.0f, MaxHealth), HealthDelta);
		ApplyHeal(HealthDelta);
	}
}

void UOceanSurvivalComponent::ApplyDamage(float Amount)
{
	if (bHasDied || Amount <= 0.0f)
	{
		return;
	}

	const float PreviousHealth = Health;
	Health = FMath::Clamp(Health - Amount, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("[TDD] Survival ApplyDamage: Health %f -> %f (damage=%f)"),
		PreviousHealth, Health, Amount);

	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(Health);
	}

	if (Health <= 0.0f && !bHasDied)
	{
		bHasDied = true;
		UE_LOG(LogTemp, Log, TEXT("[TDD] Survival Death triggered: Health=%f"), Health);
		if (OnDeath.IsBound())
		{
			OnDeath.Broadcast();
		}
	}
}

void UOceanSurvivalComponent::ApplyHeal(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	if (bHasDied)
	{
		// Optional resurrection is intentionally disallowed; healing a dead actor is a no-op.
		return;
	}

	const float PreviousHealth = Health;
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);

	if (Health != PreviousHealth)
	{
		UE_LOG(LogTemp, Log, TEXT("[TDD] Survival ApplyHeal: Health %f -> %f (heal=%f)"),
			PreviousHealth, Health, Amount);

		if (OnHealthChanged.IsBound())
		{
			OnHealthChanged.Broadcast(Health);
		}
	}
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
