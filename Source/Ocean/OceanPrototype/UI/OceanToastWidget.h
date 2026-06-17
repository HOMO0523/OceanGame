#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanToastWidget.generated.h"

UCLASS(Abstract)
class OCEAN_API UOceanToastWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 显示一条提示消息，指定持续时间（秒）。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Toast")
	void Show(const FText& Message, float Duration = 2.0f);

	/** 隐藏提示。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Toast")
	void Hide();

protected:
	/** WBP 实现：显示/隐藏视觉。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Toast")
	void OnToastStateChanged(bool bVisible, const FText& NewMessage);

private:
	/** 自动隐藏计时器。 */
	FTimerHandle AutoDismissTimer;
	void OnAutoDismiss();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
