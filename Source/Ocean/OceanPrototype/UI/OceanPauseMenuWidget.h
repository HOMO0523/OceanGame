#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPauseMenuWidget.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;

/**
 * In-game pause menu (ESC): Resume, Save (3 slots), Settings, Quit to Menu.
 */
UCLASS()
class OCEAN_API UOceanPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY() TObjectPtr<UButton> ResumeButton;
	UPROPERTY() TObjectPtr<UButton> SettingsButton;
	UPROPERTY() TObjectPtr<UButton> QuitToMenuButton;
	UPROPERTY() TObjectPtr<UVerticalBox> SaveSlotList;

	bool bIsInitialized = false;

	UFUNCTION() void OnResumeClicked();
	UFUNCTION() void OnSettingsClicked();
	UFUNCTION() void OnQuitToMenuClicked();
	UFUNCTION() void OnSaveSlotClicked(int32 SlotIndex);

	void BuildSaveSlots();
};
