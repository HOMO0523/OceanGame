// Copyright Epic Games, Inc. All Rights Reserved.

#include "OceanGameMode.h"
#include "OceanPrototype/OceanSurvivalHUD.h"

AOceanGameMode::AOceanGameMode()
{
	HUDClass = AOceanSurvivalHUD::StaticClass();
}
