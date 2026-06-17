#include "OceanPrototype/UI/OceanHUDRootWidget.h"
#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "Ocean.h"

void UOceanHUDRootWidget::SetBackpackOpen(bool bNewBackpackOpen)
{
	if (bBackpackOpen == bNewBackpackOpen)
	{
		return;
	}

	bBackpackOpen = bNewBackpackOpen;
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanUIToggleBackpack: open=%d"), bBackpackOpen ? 1 : 0);
	OnBackpackOpenChanged(bBackpackOpen);
}

void UOceanHUDRootWidget::ToggleBackpack()
{
	SetBackpackOpen(!bBackpackOpen);
}

void UOceanHUDRootWidget::BindSurvivalToStatusPanel(UOceanSurvivalComponent* InSurvival)
{
	if (StatusPanel)
	{
		StatusPanel->BindSurvivalComponent(InSurvival);
	}
	else
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanHUDRoot: BindSurvivalToStatusPanel called but StatusPanel is null (WBP未绑定)"));
	}
}
