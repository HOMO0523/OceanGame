#include "OceanLandscapeAutomationLibrary.h"

#include "Editor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Landscape.h"
#include "LandscapeComponent.h"
#include "LandscapeInfo.h"
#include "LandscapeProxy.h"

DEFINE_LOG_CATEGORY_STATIC(LogOceanEditorAutomation, Log, All);

namespace
{
constexpr TCHAR SupportLandscapeLabel[] = TEXT("Landscape_WaterSupport");
constexpr TCHAR WaterBrushManagerLabel[] = TEXT("WaterBrushManager_Prototype");
constexpr int32 ComponentCountXY = 16;
constexpr int32 SectionsPerComponent = 1;
constexpr int32 QuadsPerSection = 63;
constexpr float LandscapeScaleXY = 100.0f;
constexpr float LandscapeScaleZ = 100.0f;
constexpr float LandscapeOriginZ = -100.0f;

void AppendTdd(FString& Report, const TCHAR* Name, const FString& Message)
{
	const FString Line = FString::Printf(TEXT("[TDD] %s: %s"), Name, *Message);
	UE_LOG(LogOceanEditorAutomation, Log, TEXT("%s"), *Line);
	Report += Line;
	Report += TEXT("\n");
}

ALandscape* FindSupportLandscape(UWorld* World)
{
	for (TActorIterator<ALandscape> It(World); It; ++It)
	{
		ALandscape* Landscape = *It;
		if (Landscape && Landscape->GetActorLabel() == SupportLandscapeLabel)
		{
			return Landscape;
		}
	}

	return nullptr;
}

UClass* GetWaterBrushManagerClass()
{
	return LoadClass<AActor>(nullptr, TEXT("/Script/WaterEditor.WaterBrushManager"));
}

AActor* FindWaterBrushManager(UWorld* World, UClass* WaterBrushManagerClass)
{
	if (!WaterBrushManagerClass)
	{
		return nullptr;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Manager = *It;
		if (Manager && Manager->IsA(WaterBrushManagerClass) && Manager->GetActorLabel() == WaterBrushManagerLabel)
		{
			return Manager;
		}
	}

	return nullptr;
}

ALandscape* CreateSupportLandscape(UWorld* World)
{
	const int32 QuadsPerComponent = SectionsPerComponent * QuadsPerSection;
	const int32 SizeX = ComponentCountXY * QuadsPerComponent + 1;
	const int32 SizeY = ComponentCountXY * QuadsPerComponent + 1;
	const FVector Scale(LandscapeScaleXY, LandscapeScaleXY, LandscapeScaleZ);
	const FVector Center(0.0, 0.0, LandscapeOriginZ);
	const FVector Offset = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, Scale)
		.TransformVector(FVector(-ComponentCountXY * QuadsPerComponent / 2.0, -ComponentCountXY * QuadsPerComponent / 2.0, 0.0));

	ALandscape* Landscape = World->SpawnActor<ALandscape>(Center + Offset, FRotator::ZeroRotator);
	if (!Landscape)
	{
		return nullptr;
	}

	Landscape->SetActorLabel(SupportLandscapeLabel);
	Landscape->SetActorRelativeScale3D(Scale);
	Landscape->StaticLightingLOD = FMath::DivideAndRoundUp(FMath::CeilLogTwo((SizeX * SizeY) / (2048 * 2048) + 1), static_cast<uint32>(2));

	TArray<uint16> HeightData;
	HeightData.Init(32768, SizeX * SizeY);

	TMap<FGuid, TArray<uint16>> HeightmapDataPerLayers;
	HeightmapDataPerLayers.Add(FGuid(), MoveTemp(HeightData));

	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;
	MaterialLayerDataPerLayers.Add(FGuid(), TArray<FLandscapeImportLayerInfo>());

	TArray<FLandscapeLayer> EmptyLandscapeLayers;
	Landscape->Import(
		FGuid::NewGuid(),
		0,
		0,
		SizeX - 1,
		SizeY - 1,
		SectionsPerComponent,
		QuadsPerSection,
		HeightmapDataPerLayers,
		nullptr,
		MaterialLayerDataPerLayers,
		ELandscapeImportAlphamapType::Additive,
		MakeArrayView(EmptyLandscapeLayers));

	if (ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo())
	{
		LandscapeInfo->UpdateLayerInfoMap(Landscape);
	}

	Landscape->PostEditChange();
	Landscape->MarkPackageDirty();
	return Landscape;
}
}

FString UOceanLandscapeAutomationLibrary::EnsureWaterOceanLandscapeSupport()
{
	FString Report;
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		AppendTdd(Report, TEXT("OceanSupportLandscapeWorld"), TEXT("result=FAIL"));
		return Report;
	}

	ALandscape* Landscape = FindSupportLandscape(World);
	if (!Landscape || Landscape->LandscapeComponents.Num() == 0)
	{
		if (Landscape)
		{
			World->DestroyActor(Landscape);
		}
		Landscape = CreateSupportLandscape(World);
	}

	const bool bHasLandscape = Landscape != nullptr;
	const int32 LandscapeComponentCount = Landscape ? Landscape->LandscapeComponents.Num() : 0;
	AppendTdd(Report, TEXT("OceanSupportLandscapeExists"), bHasLandscape ? TEXT("result=PASS") : TEXT("result=FAIL"));
	AppendTdd(Report, TEXT("OceanSupportLandscapeComponentCount"), FString::Printf(TEXT("actual=%d expected=256"), LandscapeComponentCount));

	UClass* WaterBrushManagerClass = GetWaterBrushManagerClass();
	AActor* WaterBrushManager = FindWaterBrushManager(World, WaterBrushManagerClass);
	if (!WaterBrushManager)
	{
		WaterBrushManager = WaterBrushManagerClass
			? World->SpawnActor<AActor>(WaterBrushManagerClass, FVector::ZeroVector, FRotator::ZeroRotator)
			: nullptr;
		if (WaterBrushManager)
		{
			WaterBrushManager->SetActorLabel(WaterBrushManagerLabel);
			if (UFunction* SetupDefaultMaterialsFunction = WaterBrushManager->FindFunction(TEXT("SetupDefaultMaterials")))
			{
				WaterBrushManager->ProcessEvent(SetupDefaultMaterialsFunction, nullptr);
			}
			if (UFunction* ForceUpdateFunction = WaterBrushManager->FindFunction(TEXT("ForceUpdate")))
			{
				WaterBrushManager->ProcessEvent(ForceUpdateFunction, nullptr);
			}
			WaterBrushManager->MarkPackageDirty();
		}
	}

	int32 WaterBrushManagerCount = 0;
	if (WaterBrushManagerClass)
	{
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			if (It->IsA(WaterBrushManagerClass) && It->GetActorLabel() == WaterBrushManagerLabel)
			{
				++WaterBrushManagerCount;
			}
		}
	}

	AppendTdd(Report, TEXT("OceanWaterBrushManagerCount"), FString::Printf(TEXT("actual=%d expected=1"), WaterBrushManagerCount));

	if (Landscape)
	{
		Landscape->MarkPackageDirty();
	}
	World->MarkPackageDirty();

	return Report;
}
