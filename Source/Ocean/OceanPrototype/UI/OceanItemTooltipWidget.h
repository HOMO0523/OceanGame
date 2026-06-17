#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanItemTooltipWidget.generated.h"

class UTextBlock;
class UBorder;
class UVerticalBox;

UCLASS()
class OCEAN_API UOceanItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowTooltip(const FText& ItemName, const FText& ItemDescription, const FText& EffectText);
	void HideTooltip();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	bool bIsInitialized = false;
	UPROPERTY() TObjectPtr<UBorder> TooltipBorder;
	UPROPERTY() TObjectPtr<UVerticalBox> TooltipVBox;
	UPROPERTY() TObjectPtr<UTextBlock> NameText;
	UPROPERTY() TObjectPtr<UTextBlock> DescText;
	UPROPERTY() TObjectPtr<UTextBlock> EffectTextBlock;
};
