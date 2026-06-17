#include "OceanPrototype/OceanBuildComponent.h"
#include "Ocean.h"
#include "OceanPrototype/OceanBuildGridComponent.h"
#include "OceanPrototype/OceanBuildModuleActor.h"
#include "OceanPrototype/OceanBuildModuleDefinition.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "BuoyancyComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

UOceanBuildComponent::UOceanBuildComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOceanBuildComponent::ToggleBuildMode()
{
	bBuildModeActive = !bBuildModeActive;
}

void UOceanBuildComponent::SetBuildModeActive(bool bNewBuildModeActive)
{
	bBuildModeActive = bNewBuildModeActive;
}

void UOceanBuildComponent::RotatePreview()
{
	RotationQuarterTurns = (RotationQuarterTurns + 1) % 4;
}

FOceanPlacementQueryResult UOceanBuildComponent::QuerySelectedModulePlacement(const FVector& WorldLocation)
{
	FOceanPlacementQueryResult Result;
	Result.RotationQuarterTurns = RotationQuarterTurns;

	if (!bBuildModeActive)
	{
		Result.FailureReason = EOceanPlacementFailureReason::NoBuildMode;
		return Result;
	}

	if (!IsValid(SelectedModule))
	{
		Result.FailureReason = EOceanPlacementFailureReason::NoSelectedModule;
		return Result;
	}

	AOceanFloatingPlatform* ResolvedTargetPlatform = ResolveTargetPlatform();
	if (!IsValid(ResolvedTargetPlatform))
	{
		Result.FailureReason = EOceanPlacementFailureReason::NoTargetPlatform;
		return Result;
	}

	UOceanBuildGridComponent* Grid = ResolvedTargetPlatform->GetBuildGrid();
	if (!IsValid(Grid))
	{
		Result.FailureReason = EOceanPlacementFailureReason::NoTargetPlatform;
		return Result;
	}

	Result.AnchorCell = Grid->WorldToCell(WorldLocation);
	Result.SnappedWorldLocation = Grid->CellToWorld(Result.AnchorCell);
	Result.FootprintCells = SelectedModule->GetFootprintCells(Result.AnchorCell, RotationQuarterTurns);

	Result.FailureReason = Grid->ExplainFootprintPlacement(Result.FootprintCells, SelectedModule->bRequiresAdjacency);
	if (Result.FailureReason != EOceanPlacementFailureReason::None)
	{
		return Result;
	}

	AActor* OwnerActor = GetOwner();
	UOceanInventoryComponent* Inventory = OwnerActor ? OwnerActor->FindComponentByClass<UOceanInventoryComponent>() : nullptr;
	if (!IsValid(Inventory) || !Inventory->CanAfford(SelectedModule->BuildCost))
	{
		Result.FailureReason = EOceanPlacementFailureReason::InsufficientResources;
		return Result;
	}

	Result.bCanPlace = true;
	Result.FailureReason = EOceanPlacementFailureReason::None;
	return Result;
}

bool UOceanBuildComponent::TryPlaceSelectedModuleAtWorld(const FVector& WorldLocation, FText& OutMessage)
{
	LastPlacedModuleActor = nullptr;

	const FOceanPlacementQueryResult PlacementQuery = QuerySelectedModulePlacement(WorldLocation);
	if (IsValid(SelectedModule) && !PlacementQuery.FootprintCells.IsEmpty())
	{
		const int32 ExpectedFootprintCount = FMath::Max(0, SelectedModule->FootprintSize.X) * FMath::Max(0, SelectedModule->FootprintSize.Y);
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildFootprintCellCount: actual=%d expected=%d"), PlacementQuery.FootprintCells.Num(), ExpectedFootprintCount);
	}

	if (!PlacementQuery.bCanPlace)
	{
		switch (PlacementQuery.FailureReason)
		{
		case EOceanPlacementFailureReason::NoBuildMode:
			OutMessage = NSLOCTEXT("Ocean", "BuildModeInactive", "未进入建造模式");
			break;
		case EOceanPlacementFailureReason::NoSelectedModule:
			OutMessage = NSLOCTEXT("Ocean", "BuildNoSelectedModule", "未选择建造模块");
			break;
		case EOceanPlacementFailureReason::NoTargetPlatform:
			OutMessage = NSLOCTEXT("Ocean", "BuildNoTargetPlatform", "没有目标平台");
			break;
		case EOceanPlacementFailureReason::InsufficientResources:
		{
			AActor* OwnerActor = GetOwner();
			const UOceanInventoryComponent* Inventory = OwnerActor ? OwnerActor->FindComponentByClass<UOceanInventoryComponent>() : nullptr;
			OutMessage = IsValid(Inventory)
				? NSLOCTEXT("Ocean", "BuildCannotAfford", "材料不足")
				: NSLOCTEXT("Ocean", "BuildNoInventory", "没有背包");
			break;
		}
		case EOceanPlacementFailureReason::InvalidWorldHit:
		case EOceanPlacementFailureReason::OccupiedCell:
		case EOceanPlacementFailureReason::DetachedFromPlatform:
		default:
			OutMessage = NSLOCTEXT("Ocean", "BuildInvalidPlacement", "无法放置在此处");
			break;
		}
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		OutMessage = NSLOCTEXT("Ocean", "BuildNoWorld", "没有可用世界");
		return false;
	}

	AOceanFloatingPlatform* ResolvedTargetPlatform = ResolveTargetPlatform();
	UOceanBuildGridComponent* Grid = IsValid(ResolvedTargetPlatform) ? ResolvedTargetPlatform->GetBuildGrid() : nullptr;
	if (!IsValid(Grid))
	{
		OutMessage = NSLOCTEXT("Ocean", "BuildNoTargetPlatform", "没有目标平台");
		return false;
	}

	AActor* OwnerActor = GetOwner();
	UOceanInventoryComponent* Inventory = OwnerActor ? OwnerActor->FindComponentByClass<UOceanInventoryComponent>() : nullptr;
	if (!IsValid(Inventory))
	{
		OutMessage = NSLOCTEXT("Ocean", "BuildNoInventory", "没有背包");
		return false;
	}

	TSubclassOf<AOceanBuildModuleActor> ModuleClass = SelectedModule->ModuleActorClass;
	if (!ModuleClass)
	{
		ModuleClass = FallbackModuleActorClass;
		if (!ModuleClass)
		{
			ModuleClass = AOceanBuildModuleActor::StaticClass();
		}
	}

	FVector SpawnLocation = FVector::ZeroVector;
	for (const FIntPoint& Cell : PlacementQuery.FootprintCells)
	{
		SpawnLocation += Grid->CellToWorld(Cell);
	}
	SpawnLocation /= static_cast<float>(PlacementQuery.FootprintCells.Num());

	const FRotator SpawnRotation(0.0f, static_cast<float>(((RotationQuarterTurns % 4) + 4) % 4) * 90.0f, 0.0f);
	AOceanBuildModuleActor* PlacedModule = World->SpawnActor<AOceanBuildModuleActor>(ModuleClass, SpawnLocation, SpawnRotation);
	if (!PlacedModule)
	{
		OutMessage = NSLOCTEXT("Ocean", "BuildSpawnFailed", "建造模块生成失败");
		return false;
	}

	if (!Inventory->TrySpend(SelectedModule->BuildCost))
	{
		PlacedModule->Destroy();
		OutMessage = NSLOCTEXT("Ocean", "BuildSpendFailed", "材料不足");
		return false;
	}

	Grid->ReserveFootprint(PlacementQuery.FootprintCells, PlacedModule->GetFName());
	PlacedModule->SetPlaceholderMesh(SelectedModule->PreviewMesh);
	PlacedModule->ConfigurePlacedModule(SelectedModule, PlacementQuery.FootprintCells, RotationQuarterTurns);
	LastPlacedModuleActor = PlacedModule;

	const bool bHasBuoyancy = PlacedModule->FindComponentByClass<UBuoyancyComponent>() != nullptr;
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildPlacedModuleHasBuoyancy: result=%s"), bHasBuoyancy ? TEXT("PASS") : TEXT("FAIL"));

	OutMessage = NSLOCTEXT("Ocean", "BuildSuccess", "建造成功");
	return true;
}

void UOceanBuildComponent::SetSelectedModule(UOceanBuildModuleDefinition* ModuleDefinition)
{
	SelectedModule = ModuleDefinition;
}

void UOceanBuildComponent::SetTargetPlatform(AOceanFloatingPlatform* Platform)
{
	TargetPlatform = Platform;
}

void UOceanBuildComponent::SetFallbackModuleActorClass(TSubclassOf<AOceanBuildModuleActor> ActorClass)
{
	FallbackModuleActorClass = ActorClass;
}

AOceanFloatingPlatform* UOceanBuildComponent::ResolveTargetPlatform()
{
	if (IsValid(TargetPlatform))
	{
		return TargetPlatform;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<AOceanFloatingPlatform> PlatformIt(World); PlatformIt; ++PlatformIt)
	{
		AOceanFloatingPlatform* Candidate = *PlatformIt;
		if (IsValid(Candidate))
		{
			TargetPlatform = Candidate;
			return TargetPlatform;
		}
	}

	return nullptr;
}
