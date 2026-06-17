#include "OceanPrototype/UI/OceanHUDRootWidget.h"
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
