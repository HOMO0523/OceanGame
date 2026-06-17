#include "OceanPrototype/UI/OceanBuildPanelWidget.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanBuildModuleDefinition.h"
#include "Ocean.h"

void UOceanBuildPanelWidget::BindBuildComponent(UOceanBuildComponent* InBuild)
{
	BuildComponent = InBuild;

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildPanel: bound=1 active=%d rotation=%d"),
		InBuild ? (InBuild->IsBuildModeActive() ? 1 : 0) : -1,
		InBuild ? InBuild->GetRotationQuarterTurns() : -1);
}

void UOceanBuildPanelWidget::SelectModule(UOceanBuildModuleDefinition* ModuleDef)
{
	if (BuildComponent.IsValid())
	{
		BuildComponent->SetSelectedModule(ModuleDef);
	}
}

UOceanBuildModuleDefinition* UOceanBuildPanelWidget::GetSelectedModule() const
{
	return BuildComponent.IsValid() ? BuildComponent->GetSelectedModule() : nullptr;
}

int32 UOceanBuildPanelWidget::GetRotationQuarterTurns() const
{
	return BuildComponent.IsValid() ? BuildComponent->GetRotationQuarterTurns() : 0;
}

bool UOceanBuildPanelWidget::IsBuildModeActive() const
{
	return BuildComponent.IsValid() && BuildComponent->IsBuildModeActive();
}

void UOceanBuildPanelWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (!BuildComponent.IsValid())
	{
		return;
	}

	const bool bCurrentActive = BuildComponent->IsBuildModeActive();
	const int32 CurrentRotation = BuildComponent->GetRotationQuarterTurns();
	UOceanBuildModuleDefinition* CurrentModule = BuildComponent->GetSelectedModule();

	if (bCurrentActive != LastBuildModeActive || CurrentRotation != LastRotationQuarterTurns)
	{
		LastBuildModeActive = bCurrentActive;
		LastRotationQuarterTurns = CurrentRotation;
		OnBuildStateChanged(bCurrentActive, CurrentRotation);
	}

	if (CurrentModule != LastSelectedModule)
	{
		LastSelectedModule = CurrentModule;
		OnSelectedModuleChanged(CurrentModule);
	}
}
