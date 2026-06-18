#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanSettingsWidget.generated.h"

class USlider;
class UButton;
class UTextBlock;

/**
 * Settings panel: BGM volume, SFX volume.
 * Persists via UOceanSaveManager.
 */
UCLASS()
class OCEAN_API UOceanSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

protected:
	UPROPERTY() TObjectPtr<USlider> BGMSlider;
	UPROPERTY() TObjectPtr<USlider> SFXSlider;
	UPROPERTY() TObjectPtr<UButton> SaveButton;
	UPROPERTY() TObjectPtr<UButton> CloseButton;
	UPROPERTY() TObjectPtr<UTextBlock> BGMValueText;
	UPROPERTY() TObjectPtr<UTextBlock> SFXValueText;

	bool bIsInitialized = false;

	UFUNCTION() void OnBGMChanged(float Value);
	UFUNCTION() void OnSFXChanged(float Value);
	UFUNCTION() void OnSaveClicked();
	UFUNCTION() void OnCloseClicked();

	void UpdateValueLabels();
};
