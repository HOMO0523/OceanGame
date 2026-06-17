#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanTimePanelWidget.generated.h"

class UTextBlock;

UENUM(BlueprintType)
enum class EOceanTimeOfDay : uint8 { Morning, Afternoon, Night };

UCLASS()
class OCEAN_API UOceanTimePanelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetDay(int32 NewDay);
	void SetTimeOfDay(EOceanTimeOfDay NewTimeOfDay);
	void SetDayAndTime(int32 NewDay, EOceanTimeOfDay NewTimeOfDay);
	int32 GetDay() const { return Day; }
	EOceanTimeOfDay GetTimeOfDay() const { return TimeOfDay; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	int32 Day = 1;
	EOceanTimeOfDay TimeOfDay = EOceanTimeOfDay::Morning;
	UPROPERTY()
	TObjectPtr<UTextBlock> TimeText;
	void RefreshText();

	static const TCHAR* ToDisplayName(EOceanTimeOfDay T)
	{
		switch (T) { case EOceanTimeOfDay::Morning: return TEXT("上午"); case EOceanTimeOfDay::Afternoon: return TEXT("下午"); default: return TEXT("夜晚"); }
	}
};
