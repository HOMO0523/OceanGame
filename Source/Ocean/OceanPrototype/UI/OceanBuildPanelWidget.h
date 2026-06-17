#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanPrototype/OceanBuildPlacementTypes.h"
#include "OceanBuildPanelWidget.generated.h"

class UOceanBuildComponent;
class UOceanBuildModuleDefinition;
class UTextBlock;
class UVerticalBox;
class UBorder;

UCLASS()
class OCEAN_API UOceanBuildPanelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void BindBuildComponent(UOceanBuildComponent* InBuild);
	void SelectModule(UOceanBuildModuleDefinition* ModuleDef);
	UOceanBuildModuleDefinition* GetSelectedModule() const;
	int32 GetRotationQuarterTurns() const;
	bool IsBuildModeActive() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TWeakObjectPtr<UOceanBuildComponent> BuildComponent;
	bool bIsInitialized = false;
	bool LastBuildModeActive = false;
	int32 LastRotationQuarterTurns = -1;
	TObjectPtr<UOceanBuildModuleDefinition> LastSelectedModule;

	UPROPERTY() TObjectPtr<UBorder> BgBorder;
	UPROPERTY() TObjectPtr<UVerticalBox> RootBox;
	UPROPERTY() TObjectPtr<UTextBlock> ModeText;
	UPROPERTY() TObjectPtr<UTextBlock> ModuleText;
	UPROPERTY() TObjectPtr<UTextBlock> RotationText;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	void UpdateVisuals();
};
