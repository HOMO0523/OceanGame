#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanTimePanelWidget.generated.h"

/** 一天中的时段，对应策划：上午/下午/夜晚 */
UENUM(BlueprintType)
enum class EOceanTimeOfDay : uint8
{
	Morning  UMETA(DisplayName = "上午"),
	Afternoon UMETA(DisplayName = "下午"),
	Night    UMETA(DisplayName = "夜晚")
};

UCLASS(Abstract)
class OCEAN_API UOceanTimePanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 设置当前天数。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Time")
	void SetDay(int32 NewDay);

	/** 设置当前时段。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Time")
	void SetTimeOfDay(EOceanTimeOfDay NewTimeOfDay);

	/** 同时设置天数和时段。 */
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI|Time")
	void SetDayAndTime(int32 NewDay, EOceanTimeOfDay NewTimeOfDay);

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Time")
	int32 GetDay() const { return Day; }

	UFUNCTION(BlueprintPure, Category = "Ocean|UI|Time")
	EOceanTimeOfDay GetTimeOfDay() const { return TimeOfDay; }

protected:
	/** WBP 实现：时间变化时刷新视觉。 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI|Time")
	void OnTimeUpdated(int32 NewDay, EOceanTimeOfDay NewTimeOfDay);

private:
	int32 Day = 1;
	EOceanTimeOfDay TimeOfDay = EOceanTimeOfDay::Morning;
};
