#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OceanSurvivalHUD.generated.h"

UCLASS()
class OCEAN_API AOceanSurvivalHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawDebugLine(const FString& Text, const FColor& Color, float X, float& Y);
};
