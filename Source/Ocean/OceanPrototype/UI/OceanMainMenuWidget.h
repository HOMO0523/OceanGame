#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanMainMenuWidget.generated.h"

class UButton;
class UVerticalBox;
class UOceanSaveSlotWidget;

/**
 * Main menu shown when the game starts or when returning to menu.
 * Contains: New Game, Continue (with slot list), Settings, Quit.
 */
UCLASS()
class OCEAN_API UOceanMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

protected:
	UPROPERTY() TObjectPtr<UButton> NewGameButton;
	UPROPERTY() TObjectPtr<UButton> SettingsButton;
	UPROPERTY() TObjectPtr<UButton> QuitButton;
	UPROPERTY() TObjectPtr<UVerticalBox> SlotList;

	bool bIsInitialized = false;

	UFUNCTION() void OnNewGameClicked();
	UFUNCTION() void OnSettingsClicked();
	UFUNCTION() void OnQuitClicked();
	UFUNCTION() void OnSlotLoadClicked(int32 SlotIndex);
	UFUNCTION() void OnSlotDeleteClicked(int32 SlotIndex);

	void RefreshSlots();
};
