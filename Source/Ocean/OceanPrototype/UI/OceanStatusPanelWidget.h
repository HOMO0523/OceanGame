#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanStatusPanelWidget.generated.h"

class UOceanSurvivalComponent;
class UVerticalBox;
class UHorizontalBox;
class UProgressBar;
class UBorder;
class UTextBlock;

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

	bool bIsInitialized = false;

	// Last logged values (integer percent for log throttling)
	int32 LastLoggedStaminaPct = -1;
	int32 LastLoggedHydrationPct = -1;
	int32 LastLoggedSatietyPct = -1;

	// Visual elements
	UPROPERTY() TObjectPtr<UVerticalBox> RootBox;
	UPROPERTY() TObjectPtr<UHorizontalBox> StaminaCellRow;
	UPROPERTY() TArray<TObjectPtr<UBorder>> StaminaCells;
	UPROPERTY() TObjectPtr<UProgressBar> HydrationBar;
	UPROPERTY() TObjectPtr<UProgressBar> SatietyBar;
	UPROPERTY() TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY() TObjectPtr<UTextBlock> StaminaLabel;
	UPROPERTY() TObjectPtr<UTextBlock> HydrationLabel;
	UPROPERTY() TObjectPtr<UTextBlock> SatietyLabel;
	UPROPERTY() TObjectPtr<UTextBlock> HealthLabel;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	void CheckForChanges();
	void UpdateVisuals();
};
