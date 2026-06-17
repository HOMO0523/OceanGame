#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanHUDRootWidget.generated.h"

UCLASS()
class OCEAN_API UOceanHUDRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void SetBackpackOpen(bool bNewBackpackOpen);

	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void ToggleBackpack();

	UFUNCTION(BlueprintPure, Category = "Ocean|UI")
	bool IsBackpackOpen() const { return bBackpackOpen; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI")
	void OnBackpackOpenChanged(bool bNewBackpackOpen);

private:
	UPROPERTY(VisibleAnywhere, Category = "Ocean|UI")
	bool bBackpackOpen = false;
};
