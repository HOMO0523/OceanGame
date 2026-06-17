#include "OceanPrototype/OceanSurvivalHUD.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanInteractableInterface.h"
#include "OceanPrototype/OceanInteractionComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanResourceTypes.h"
#include "OceanPrototype/OceanSurvivalComponent.h"

namespace
{
bool HasBlueprintInteractionTextOverride(AActor* Candidate, const FName FunctionName)
{
	const UFunction* Function = Candidate ? Candidate->FindFunction(FunctionName) : nullptr;
	return Function && !Function->GetOwnerClass()->HasAnyClassFlags(CLASS_Native);
}

FText DispatchGetOceanInteractionText(AActor* Target, AActor* Interactor)
{
	static const FName GetInteractionTextName(TEXT("GetOceanInteractionText"));
	if (HasBlueprintInteractionTextOverride(Target, GetInteractionTextName))
	{
		return IOceanInteractableInterface::Execute_GetOceanInteractionText(Target, Interactor);
	}

	if (const IOceanInteractableInterface* NativeInterface = Cast<IOceanInteractableInterface>(Target))
	{
		return NativeInterface->GetOceanInteractionText_Implementation(Interactor);
	}

	return IOceanInteractableInterface::Execute_GetOceanInteractionText(Target, Interactor);
}
}

void AOceanSurvivalHUD::DrawHUD()
{
	Super::DrawHUD();

	APawn* Pawn = PlayerOwner ? PlayerOwner->GetPawn() : nullptr;
	if (!Canvas || !Pawn)
	{
		return;
	}

	constexpr float X = 40.0f;
	float Y = 40.0f;

	if (const UOceanSurvivalComponent* Survival = Pawn->FindComponentByClass<UOceanSurvivalComponent>())
	{
		DrawDebugLine(
			FString::Printf(TEXT("体力 %.0f | 水分 %.0f | 饱食 %.0f"),
				Survival->GetStamina(),
				Survival->GetHydration(),
				Survival->GetSatiety()),
			FColor::White,
			X,
			Y);
	}

	if (const UOceanInventoryComponent* Inventory = Pawn->FindComponentByClass<UOceanInventoryComponent>())
	{
		DrawDebugLine(
			FString::Printf(TEXT("背包 木材 %d | 碎片 %d | 食物 %d | 水 %d"),
				Inventory->GetResourceAmount(EOceanResourceType::Wood),
				Inventory->GetResourceAmount(EOceanResourceType::Scrap),
				Inventory->GetResourceAmount(EOceanResourceType::Food),
				Inventory->GetResourceAmount(EOceanResourceType::Water)),
			FColor::Cyan,
			X,
			Y);
	}

	if (const UOceanBuildComponent* Build = Pawn->FindComponentByClass<UOceanBuildComponent>())
	{
		DrawDebugLine(
			Build->IsBuildModeActive()
				? TEXT("建造模式：左键放置 | R 旋转 | B 退出")
				: TEXT("B 打开建造模式"),
			FColor::Yellow,
			X,
			Y);
	}

	if (const UOceanInteractionComponent* Interaction = Pawn->FindComponentByClass<UOceanInteractionComponent>())
	{
		if (AActor* Target = Interaction->FindBestInteractable())
		{
			DrawDebugLine(DispatchGetOceanInteractionText(Target, Pawn).ToString(), FColor::Green, X, Y);
		}
		else
		{
			DrawDebugLine(TEXT("F：附近没有可交互目标"), FColor::Silver, X, Y);
		}
	}
}

void AOceanSurvivalHUD::DrawDebugLine(const FString& Text, const FColor& Color, float X, float& Y)
{
	DrawText(Text, Color, X, Y);
	Y += 24.0f;
}
