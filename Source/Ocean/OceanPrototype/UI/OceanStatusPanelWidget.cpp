#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "Ocean.h"

void UOceanStatusPanelWidget::BindSurvivalComponent(UOceanSurvivalComponent* InSurvival)
{
	SurvivalComponent = InSurvival;

	if (SurvivalComponent.IsValid())
	{
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanStatusPanel: bound=1 stamina=%.1f hydration=%.1f satiety=%.1f"),
			SurvivalComponent->GetStamina(),
			SurvivalComponent->GetHydration(),
			SurvivalComponent->GetSatiety());

		OnSurvivalBound();
		BroadcastIfChanged();
	}
	else
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanStatusPanel: bound=0 (null survival component)"));
	}
}

void UOceanStatusPanelWidget::UnbindSurvivalComponent()
{
	SurvivalComponent = nullptr;
	LastStaminaPercent = -1.0f;
	LastHydrationPercent = -1.0f;
	LastSatietyPercent = -1.0f;
	LastStaminaCells = -1;
}

float UOceanStatusPanelWidget::GetStaminaPercent() const
{
	if (!SurvivalComponent.IsValid())
	{
		return 0.0f;
	}
	// 上限100，百分比 = Stamina/100
	return FMath::Clamp(SurvivalComponent->GetStamina() / 100.0f, 0.0f, 1.0f);
}

float UOceanStatusPanelWidget::GetHydrationPercent() const
{
	if (!SurvivalComponent.IsValid())
	{
		return 0.0f;
	}
	return FMath::Clamp(SurvivalComponent->GetHydration() / 100.0f, 0.0f, 1.0f);
}

float UOceanStatusPanelWidget::GetSatietyPercent() const
{
	if (!SurvivalComponent.IsValid())
	{
		return 0.0f;
	}
	return FMath::Clamp(SurvivalComponent->GetSatiety() / 100.0f, 0.0f, 1.0f);
}

int32 UOceanStatusPanelWidget::GetStaminaCells() const
{
	if (!SurvivalComponent.IsValid())
	{
		return 0;
	}
	// 向上取整：0.01→1格，33.3→1格，33.4→2格，100→3格
	const float Stamina = SurvivalComponent->GetStamina();
	if (Stamina <= 0.0f)
	{
		return 0;
	}
	return FMath::Min(MaxStaminaCells, FMath::CeilToInt(Stamina / StaminaPerCell));
}

void UOceanStatusPanelWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	BroadcastIfChanged();
}

void UOceanStatusPanelWidget::BroadcastIfChanged()
{
	if (!SurvivalComponent.IsValid())
	{
		return;
	}

	const float CurrentStaminaPercent = GetStaminaPercent();
	const float CurrentHydrationPercent = GetHydrationPercent();
	const float CurrentSatietyPercent = GetSatietyPercent();
	const int32 CurrentStaminaCells = GetStaminaCells();

	// 仅在任意值变化时才调用 Blueprint 事件，减少跨语言开销
	if (!FMath::IsNearlyEqual(CurrentStaminaPercent, LastStaminaPercent) ||
		!FMath::IsNearlyEqual(CurrentHydrationPercent, LastHydrationPercent) ||
		!FMath::IsNearlyEqual(CurrentSatietyPercent, LastSatietyPercent) ||
		CurrentStaminaCells != LastStaminaCells)
	{
		LastStaminaPercent = CurrentStaminaPercent;
		LastHydrationPercent = CurrentHydrationPercent;
		LastSatietyPercent = CurrentSatietyPercent;
		LastStaminaCells = CurrentStaminaCells;

		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanStatusPanel: updated stamina=%.1f hydration=%.1f satiety=%.1f cells=%d"),
			SurvivalComponent->GetStamina(),
			SurvivalComponent->GetHydration(),
			SurvivalComponent->GetSatiety(),
			CurrentStaminaCells);

		OnStatsUpdated(CurrentStaminaPercent, CurrentHydrationPercent, CurrentSatietyPercent, CurrentStaminaCells);
	}
}
