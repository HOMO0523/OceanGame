#include "OceanPrototype/UI/OceanToastWidget.h"
#include "Ocean.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UOceanToastWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UOceanToastWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoDismissTimer);
	}
	Super::NativeDestruct();
}

void UOceanToastWidget::Show(const FText& Message, float Duration)
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanToast: show=%s duration=%.1f"), *Message.ToString(), Duration);

	OnToastStateChanged(true, Message);

	// 自动隐藏计时
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoDismissTimer);
		World->GetTimerManager().SetTimer(AutoDismissTimer, this, &UOceanToastWidget::OnAutoDismiss, Duration, false);
	}
}

void UOceanToastWidget::Hide()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoDismissTimer);
	}
	OnToastStateChanged(false, FText::GetEmpty());
}

void UOceanToastWidget::OnAutoDismiss()
{
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanToast: auto_dismiss"));
	OnToastStateChanged(false, FText::GetEmpty());
}
