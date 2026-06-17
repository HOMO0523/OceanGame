#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanStatusPanelWidget.generated.h"

class UOceanSurvivalComponent;

/**
 * 状态面板 Widget —— 显示体力/水分/饱食度。
 *
 * 设计原则：
 * - C++ 层只管数据绑定和百分比计算，不碰视觉。
 * - WBP 层通过 BlueprintImplementableEvent 接收更新通知，用进度条/文字呈现。
 * - 体力按策划"3格"设计：上限100，每格≈33.3，GetStaminaCells() 返回整数格数。
 */
UCLASS(Abstract)
class OCEAN_API UOceanStatusPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 绑定生存组件，开始监听数值变化。传入 nullptr 会安全解绑。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Status")
	void BindSurvivalComponent(UOceanSurvivalComponent* InSurvival);

	/** 解绑当前生存组件。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Status")
	void UnbindSurvivalComponent();

	// --- BlueprintPure getters：供 WBP 进度条绑定 ---

	/** 体力百分比 [0,1] */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	float GetStaminaPercent() const;

	/** 水分百分比 [0,1] */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	float GetHydrationPercent() const;

	/** 饱食度百分比 [0,1] */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	float GetSatietyPercent() const;

	/** 体力格数（上限100，每格33.3，向上取整显示） */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	int32 GetStaminaCells() const;

	/** 体力上限格数 */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	int32 GetMaxStaminaCells() const { return MaxStaminaCells; }

protected:
	/** WBP 实现：数值变化时刷新视觉。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Status")
	void OnStatsUpdated(float StaminaPercent, float HydrationPercent, float SatietyPercent, int32 StaminaCells);

	/** WBP 实现：首次绑定成功时初始化。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Status")
	void OnSurvivalBound();

private:
	/** 弱引用生存组件，避免阻挡 GC。 */
	UPROPERTY(Transient)
	TWeakObjectPtr<UOceanSurvivalComponent> SurvivalComponent;

	/** 体力一格对应的数值。100/3≈33.33 */
	static constexpr float StaminaPerCell = 33.3333f;

	/** 体力总格数上限。策划要求3格。 */
	static constexpr int32 MaxStaminaCells = 3;

	/** 缓存上次广播的值，避免每帧重复调用 Blueprint 事件。 */
	float LastStaminaPercent = -1.0f;
	float LastHydrationPercent = -1.0f;
	float LastSatietyPercent = -1.0f;
	int32 LastStaminaCells = -1;

	/** 每帧检查数值是否变化，变化时通知 WBP。 */
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	void BroadcastIfChanged();
};
