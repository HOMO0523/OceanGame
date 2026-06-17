#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "OceanDayNightCycleComponent.generated.h"

class ADirectionalLight;
class ASkyLight;

/**
 * Day/night cycle controller.
 * Adjusts directional light angle, color, and intensity based on EOceanTimeOfDay.
 * Morning: warm low sun (east, 30°), Afternoon: warmer west sun (45°),
 * Night: cold moonlight (overhead, dim blue).
 */
UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanDayNightCycleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanDayNightCycleComponent();

	virtual void BeginPlay() override;

	/** Set the current time of day and update lighting immediately. */
	UFUNCTION(BlueprintCallable, Category = "Ocean|DayNight")
	void SetTimeOfDay(EOceanTimeOfDay NewTime);

	UFUNCTION(BlueprintPure, Category = "Ocean|DayNight")
	EOceanTimeOfDay GetTimeOfDay() const { return CurrentTime; }

protected:
	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Light")
	TObjectPtr<ADirectionalLight> DirectionalLight;

	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Light")
	TObjectPtr<ASkyLight> SkyLight;

	// Morning: warm sunrise from east, low angle
	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Morning")
	FRotator MorningLightRotation = FRotator(-30.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Morning")
	FLinearColor MorningLightColor = FLinearColor(1.0f, 0.85f, 0.6f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Morning")
	float MorningLightIntensity = 3.0f;

	// Afternoon: warm west sun, higher angle
	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Afternoon")
	FRotator AfternoonLightRotation = FRotator(-45.0f, 270.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Afternoon")
	FLinearColor AfternoonLightColor = FLinearColor(1.0f, 0.75f, 0.45f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Afternoon")
	float AfternoonLightIntensity = 4.0f;

	// Night: cold moonlight, very dim
	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Night")
	FRotator NightLightRotation = FRotator(-60.0f, 180.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Night")
	FLinearColor NightLightColor = FLinearColor(0.3f, 0.4f, 0.7f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Night")
	float NightLightIntensity = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Ocean|DayNight|Night")
	float NightSkyIntensity = 0.2f;

private:
	EOceanTimeOfDay CurrentTime = EOceanTimeOfDay::Morning;

	void FindLights();
	void ApplyLighting();
};
