#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanBuildPlacementTypes.h"
#include "OceanBuildPanelWidget.generated.h"

class UOceanBuildComponent;
class UOceanBuildModuleDefinition;

UCLASS()
class OCEAN_API UOceanBuildPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 绑定建造组件。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Build")
	void BindBuildComponent(UOceanBuildComponent* InBuild);

	/** 选择建造模块。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Build")
	void SelectModule(UOceanBuildModuleDefinition* ModuleDef);

	/** 获取当前选中模块。 */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Build")
	UOceanBuildModuleDefinition* GetSelectedModule() const;

	/** 获取当前旋转（0-3）。 */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Build")
	int32 GetRotationQuarterTurns() const;

	/** 是否处于建造模式。 */
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Build")
	bool IsBuildModeActive() const;

protected:
	/** WBP 实现：建造状态变化。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Build")
	void OnBuildStateChanged(bool bNewBuildModeActive, int32 NewRotation);

	/** WBP 实现：选中模块变化。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Build")
	void OnSelectedModuleChanged(UOceanBuildModuleDefinition* NewModule);

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<UOceanBuildComponent> BuildComponent;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	bool LastBuildModeActive = false;
	int32 LastRotationQuarterTurns = -1;
	TObjectPtr<UOceanBuildModuleDefinition> LastSelectedModule;
};
