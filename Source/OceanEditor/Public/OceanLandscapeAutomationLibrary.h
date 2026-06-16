#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "OceanLandscapeAutomationLibrary.generated.h"

UCLASS()
class OCEANEDITOR_API UOceanLandscapeAutomationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Ocean|Editor Automation")
	static FString EnsureWaterOceanLandscapeSupport();
};
