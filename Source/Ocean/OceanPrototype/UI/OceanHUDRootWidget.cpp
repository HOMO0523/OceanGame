#include "OceanPrototype/UI/OceanHUDRootWidget.h"
#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/UI/OceanBuildPanelWidget.h"
#include "OceanPrototype/UI/OceanToastWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "Ocean.h"

void UOceanHUDRootWidget::SetBackpackOpen(bool bNewBackpackOpen)
{
	if (bBackpackOpen == bNewBackpackOpen) return;
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
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanHUDRoot: StatusPanel is null, skipping bind"));
	}
}

void UOceanHUDRootWidget::BindInventoryToBackpackPanel(UOceanInventoryComponent* InInventory, UOceanSurvivalComponent* InSurvival)
{
	if (BackpackPanel)
	{
		BackpackPanel->BindInventory(InInventory, InSurvival);
	}
	else
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanHUDRoot: BackpackPanel is null, skipping bind"));
	}
}

void UOceanHUDRootWidget::BindBuildToBuildPanel(UOceanBuildComponent* InBuild)
{
	if (BuildPanel)
	{
		BuildPanel->BindBuildComponent(InBuild);
	}
	else
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanHUDRoot: BuildPanel is null, skipping bind"));
	}
}

void UOceanHUDRootWidget::ShowToast(const FText& Message, float Duration)
{
	if (Toast)
	{
		Toast->Show(Message, Duration);
	}
	else
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanHUDRoot: Toast is null, message=%s"), *Message.ToString());
	}
}
