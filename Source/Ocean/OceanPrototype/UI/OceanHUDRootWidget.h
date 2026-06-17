#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanHUDRootWidget.generated.h"

class UOceanStatusPanelWidget;
class UOceanBackpackPanelWidget;
class UOceanBuildPanelWidget;
class UOceanTimePanelWidget;
class UOceanItemUseModalWidget;
class UOceanToastWidget;
class UOceanSurvivalComponent;
class UOceanInventoryComponent;
class UOceanBuildComponent;

UCLASS(Abstract)
class OCEAN_API UOceanHUDRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- 背包 ---
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void SetBackpackOpen(bool bNewBackpackOpen);

	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void ToggleBackpack();

	UFUNCTION(BlueprintPure, Category = "Ocean|UI")
	bool IsBackpackOpen() const { return bBackpackOpen; }

	// --- 数据绑定 ---
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void BindSurvivalToStatusPanel(UOceanSurvivalComponent* InSurvival);

	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void BindInventoryToBackpackPanel(UOceanInventoryComponent* InInventory, UOceanSurvivalComponent* InSurvival);

	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void BindBuildToBuildPanel(UOceanBuildComponent* InBuild);

	// --- 提示 ---
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void ShowToast(const FText& Message, float Duration = 2.0f);

	// --- 获取子面板 ---
	UFUNCTION(BlueprintPure, Category = "Ocean|UI")
	UOceanStatusPanelWidget* GetStatusPanel() const { return StatusPanel; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI")
	UOceanBackpackPanelWidget* GetBackpackPanel() const { return BackpackPanel; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI")
	UOceanBuildPanelWidget* GetBuildPanel() const { return BuildPanel; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI")
	UOceanTimePanelWidget* GetTimePanel() const { return TimePanel; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI")
	void OnBackpackOpenChanged(bool bNewBackpackOpen);

	// --- BindWidgetOptional 子面板（WBP 中未绑定时为 null）---
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "Ocean|UI")
	TObjectPtr<UOceanStatusPanelWidget> StatusPanel = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "Ocean|UI")
	TObjectPtr<UOceanBackpackPanelWidget> BackpackPanel = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "Ocean|UI")
	TObjectPtr<UOceanBuildPanelWidget> BuildPanel = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "Ocean|UI")
	TObjectPtr<UOceanTimePanelWidget> TimePanel = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "Ocean|UI")
	TObjectPtr<UOceanItemUseModalWidget> ItemUseModal = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "Ocean|UI")
	TObjectPtr<UOceanToastWidget> Toast = nullptr;

private:
	bool bBackpackOpen = false;
};
