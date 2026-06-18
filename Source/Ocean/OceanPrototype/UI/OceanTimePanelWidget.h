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
	UFUNCTION(BlueprintCallable, Category = "Ocean|Time")
	void SetDay(int32 NewDay);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Time")
	void SetTimeOfDay(EOceanTimeOfDay NewTimeOfDay);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Time")
	void SetDayAndTime(int32 NewDay, EOceanTimeOfDay NewTimeOfDay);
	int32 GetDay() const { return Day; }
	EOceanTimeOfDay GetTimeOfDay() const { return TimeOfDay; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	int32 Day = 1;
	EOceanTimeOfDay TimeOfDay = EOceanTimeOfDay::Morning;
	bool bIsInitialized = false;
	UPROPERTY() TObjectPtr<UTextBlock> TimeText;
	void RefreshText();

	static const TCHAR* ToDisplayName(EOceanTimeOfDay T)
	{
		switch (T) { case EOceanTimeOfDay::Morning: return TEXT("Morning"); case EOceanTimeOfDay::Afternoon: return TEXT("Noon"); default: return TEXT("Night"); }
	}
};
