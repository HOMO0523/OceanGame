#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanHUDRootWidget.generated.h"

class UOceanStatusPanelWidget;
class UOceanSurvivalComponent;

/**
 * HUD 根 Widget —— 管理所有 HUD 子面板的容器。
 *
 * 职责边界：
 * - 拥有子面板的引用，负责创建和绑定。
 * - 不直接操作游戏逻辑；游戏逻辑通过 Controller 调用此类的接口。
 * - WBP 层通过命名 Slot 容纳子面板。
 */
UCLASS(Abstract)
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

	/** 绑定生存组件到 StatusPanel（如果存在）。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void BindSurvivalToStatusPanel(UOceanSurvivalComponent* InSurvival);

	/** 获取 StatusPanel 子部件引用（可能为空，WBP 未赋值时）。 */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI")
	UOceanStatusPanelWidget* GetStatusPanel() const { return StatusPanel; }

protected:
	/** WBP 实现：背包开关状态变化时刷新视觉。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI")
	void OnBackpackOpenChanged(bool bNewBackpackOpen);

	/** WBP 中通过 BindWidgetOptional 绑定的 StatusPanel 子部件（可选，未绑定时为 null）。 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "Ocean|UI")
	TObjectPtr<UOceanStatusPanelWidget> StatusPanel = nullptr;

private:
	UPROPERTY(VisibleAnywhere, Category = "Ocean|UI")
	bool bBackpackOpen = false;
};
