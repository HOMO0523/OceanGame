#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanToastWidget.generated.h"

class UTextBlock;
class UBorder;

UCLASS()
class OCEAN_API UOceanToastWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void Show(const FText& Message, float Duration = 2.0f);
	void Hide();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	FTimerHandle AutoDismissTimer;
	void OnAutoDismiss();
	UPROPERTY() TObjectPtr<UBorder> ToastBorder;
	UPROPERTY() TObjectPtr<UTextBlock> ToastText;
};
