#include "OceanPrototype/OceanDayNightCycleComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Ocean.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"

UOceanDayNightCycleComponent::UOceanDayNightCycleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOceanDayNightCycleComponent::BeginPlay()
{
	Super::BeginPlay();
	FindLights();
	ApplyLighting();
}

void UOceanDayNightCycleComponent::FindLights()
{
	if (DirectionalLight && SkyLight) return;

	UWorld* World = GetWorld();
	if (!World) return;

	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		DirectionalLight = *It;
		break;
	}

	for (TActorIterator<ASkyLight> It(World); It; ++It)
	{
		SkyLight = *It;
		break;
	}

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDayNight: found_lights dir=%d sky=%d"),
		DirectionalLight ? 1 : 0, SkyLight ? 1 : 0);
}

void UOceanDayNightCycleComponent::SetTimeOfDay(EOceanTimeOfDay NewTime)
{
	if (CurrentTime == NewTime) return;
	CurrentTime = NewTime;
	ApplyLighting();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanDayNight: time=%d applied"), static_cast<int32>(CurrentTime));
}

void UOceanDayNightCycleComponent::ApplyLighting()
{
	FRotator Rot;
	FLinearColor Color;
	float Intensity = 3.0f;
	float SkyIntensity = 1.0f;

	switch (CurrentTime)
	{
	case EOceanTimeOfDay::Morning:
		Rot = MorningLightRotation;
		Color = MorningLightColor;
		Intensity = MorningLightIntensity;
		break;
	case EOceanTimeOfDay::Afternoon:
		Rot = AfternoonLightRotation;
		Color = AfternoonLightColor;
		Intensity = AfternoonLightIntensity;
		break;
	case EOceanTimeOfDay::Night:
		Rot = NightLightRotation;
		Color = NightLightColor;
		Intensity = NightLightIntensity;
		SkyIntensity = NightSkyIntensity;
		break;
	}

	if (DirectionalLight)
	{
		DirectionalLight->SetActorRotation(Rot);

		if (UDirectionalLightComponent* Comp = Cast<UDirectionalLightComponent>(DirectionalLight->GetLightComponent()))
		{
			Comp->SetLightColor(Color);
			Comp->SetIntensity(Intensity);
		}
	}

	if (SkyLight)
	{
		if (USkyLightComponent* Comp = Cast<USkyLightComponent>(SkyLight->GetLightComponent()))
		{
			Comp->SetIntensity(CurrentTime == EOceanTimeOfDay::Night ? NightSkyIntensity : 1.0f);
		}
	}
}
