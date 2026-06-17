#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "OceanStatusPanelWidget.generated.h"

class UOceanSurvivalComponent;
class UVerticalBox;

UCLASS()
class OCEAN_API UOceanStatusPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindSurvivalComponent(UOceanSurvivalComponent* InSurvival);
	void UnbindSurvivalComponent();

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	float GetStaminaPercent() const;
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	float GetHydrationPercent() const;
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	float GetSatietyPercent() const;
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	int32 GetStaminaCells() const;
	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Status")
	int32 GetMaxStaminaCells() const { return MaxStaminaCells; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TWeakObjectPtr<UOceanSurvivalComponent> SurvivalComponent;
	static constexpr float StaminaPerCell = 33.3333f;
	static constexpr int32 MaxStaminaCells = 3;
	float LastStaminaPercent = -1.0f, LastHydrationPercent = -1.0f, LastSatietyPercent = -1.0f;
	int32 LastStaminaCells = -1;

	UPROPERTY()
	TObjectPtr<UVerticalBox> RootBox;
	UPROPERTY()
	TObjectPtr<UTextBlock> StaminaText;
	UPROPERTY()
	TObjectPtr<UTextBlock> HydrationText;
	UPROPERTY()
	TObjectPtr<UTextBlock> SatietyText;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	void BroadcastIfChanged();
	void UpdateVisuals();
};
