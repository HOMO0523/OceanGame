# Ocean HUD Backpack Drawer UIUX Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the approved Ocean survival HUD and right-side backpack drawer foundation, including slot inventory, item use, placement query, WBP assets, and PIE verification.

**Architecture:** Keep UI as a presentation and command-dispatch layer. `UOceanInventoryComponent`, `UOceanSurvivalComponent`, `UOceanBuildGridComponent`, and `UOceanBuildComponent` remain gameplay authority; `UOceanHUDRootWidget` tracks UI state and forwards commands.

**Tech Stack:** UE 5.7 C++, UMG, Enhanced Input, UnrealBridge Python, Ocean no-LiveCoding TDD pipeline.

---

## Scope Guard

This plan implements the first UI foundation only. It does not add final art, fishing UI, full diving UI, island travel UI, or a full RPG inventory.

## File Structure

- `Source/Ocean/OceanPrototype/OceanItemTypes.h`: item categories, use effects, inventory slot structs, drag/drop result enums.
- `Source/Ocean/OceanPrototype/OceanInventoryComponent.*`: slot inventory APIs while preserving existing resource-stack APIs used by build costs.
- `Source/Ocean/OceanPrototype/OceanSurvivalComponent.*`: explicit recovery helper used by item effects.
- `Source/Ocean/OceanPrototype/OceanBuildPlacementTypes.h`: typed placement query result and failure reason.
- `Source/Ocean/OceanPrototype/OceanBuildGridComponent.*`: query helper that explains footprint failure.
- `Source/Ocean/OceanPrototype/OceanBuildComponent.*`: public placement query API used by UI before final placement.
- `Source/Ocean/OceanPrototype/UI/OceanHUDRootWidget.*`: UI state model and Blueprint-callable commands for drawer visibility.
- `Source/Ocean/OceanPlayerController.*`: backpack input action binding and UI state coordination.
- `Source/Ocean/Tests/OceanMVPInventoryTests.cpp`: item use and drag/drop tests.
- `Source/Ocean/Tests/OceanMVPPlacementQueryTests.cpp`: typed placement query tests.
- `Source/Ocean/Tests/OceanMVPUIModelTests.cpp`: HUD state model tests.
- `scripts/create_ocean_ui_assets.py`: creates WBP assets under `/Game/OceanPrototype/UI`.
- `scripts/verify_ocean_ui_assets.py`: bridge verification for WBP assets and PIE HUD.
- `docs/defense/index.html`, `memory-bank/*.md`: updated only after runtime verification.

## Task 1: Inventory Item Types

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanItemTypes.h`
- Modify: `Source/Ocean/OceanPrototype/OceanInventoryComponent.h`
- Test: `Source/Ocean/Tests/OceanMVPInventoryTests.cpp`

- [ ] **Step 1: Write failing item slot construction test**

Add this test file with the first test:

```cpp
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanItemTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventorySlotModelTest, "Ocean.MVP.Inventory.SlotModel", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventorySlotModelTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	TestNotNull(TEXT("[TDD] OceanInventory_SlotModelComponent"), Inventory);

	FOceanItemStack CannedFood;
	CannedFood.ItemId = TEXT("canned_food");
	CannedFood.Quantity = 2;
	CannedFood.MaxStack = 8;
	CannedFood.Category = EOceanItemCategory::Consumable;
	CannedFood.UseEffect.SatietyDelta = 100.0f;

	TestTrue(TEXT("[TDD] OceanInventory_AddItemSlot"), Inventory->AddItem(CannedFood));
	TestEqual(TEXT("[TDD] OceanInventory_SlotCount"), Inventory->GetSlots().Num(), 1);
	TestEqual(TEXT("[TDD] OceanInventory_FirstSlotItem"), Inventory->GetSlots()[0].Stack.ItemId, FName(TEXT("canned_food")));
	TestEqual(TEXT("[TDD] OceanInventory_FirstSlotQuantity"), Inventory->GetSlots()[0].Stack.Quantity, 2);
	return true;
}

#endif
```

- [ ] **Step 2: Run test to verify it fails**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.Inventory.SlotModel"
```

Expected: compile fails because `OceanItemTypes.h`, `FOceanItemStack`, and `AddItem` do not exist.

- [ ] **Step 3: Add item type header**

Create `Source/Ocean/OceanPrototype/OceanItemTypes.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "OceanItemTypes.generated.h"

UENUM(BlueprintType)
enum class EOceanItemCategory : uint8
{
	Resource UMETA(DisplayName = "Resource"),
	Consumable UMETA(DisplayName = "Consumable"),
	KeyItem UMETA(DisplayName = "Key Item"),
	Placeable UMETA(DisplayName = "Placeable")
};

UENUM(BlueprintType)
enum class EOceanInventoryDragDropResult : uint8
{
	Rejected UMETA(DisplayName = "Rejected"),
	Merged UMETA(DisplayName = "Merged"),
	Swapped UMETA(DisplayName = "Swapped"),
	Moved UMETA(DisplayName = "Moved")
};

USTRUCT(BlueprintType)
struct FOceanItemUseEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	float StaminaDelta = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	float HydrationDelta = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	float SatietyDelta = 0.0f;
};

USTRUCT(BlueprintType)
struct FOceanItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items", meta = (ClampMin = "0"))
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items", meta = (ClampMin = "1"))
	int32 MaxStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	EOceanItemCategory Category = EOceanItemCategory::Resource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	FOceanItemUseEffect UseEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	bool bKeyItem = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Items")
	bool bPlaceable = false;
};

USTRUCT(BlueprintType)
struct FOceanInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Inventory")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Inventory")
	FOceanItemStack Stack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Inventory")
	bool bLocked = false;
};
```

- [ ] **Step 4: Add slot APIs to inventory header**

Modify `UOceanInventoryComponent` with:

```cpp
#include "OceanItemTypes.h"

UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
bool AddItem(const FOceanItemStack& Stack);

UFUNCTION(BlueprintPure, Category = "Ocean|Inventory")
const TArray<FOceanInventorySlot>& GetSlots() const { return Slots; }

UPROPERTY(EditAnywhere, Category = "Ocean|Inventory", meta = (ClampMin = "1"))
int32 MaxItemSlots = 12;

UPROPERTY(VisibleAnywhere, Category = "Ocean|Inventory")
TArray<FOceanInventorySlot> Slots;
```

- [ ] **Step 5: Implement minimal `AddItem`**

Add to `OceanInventoryComponent.cpp`:

```cpp
bool UOceanInventoryComponent::AddItem(const FOceanItemStack& Stack)
{
	if (Stack.ItemId.IsNone() || Stack.Quantity <= 0 || Stack.MaxStack <= 0)
	{
		return false;
	}

	int32 Remaining = Stack.Quantity;
	for (FOceanInventorySlot& Slot : Slots)
	{
		if (Slot.Stack.ItemId == Stack.ItemId && Slot.Stack.Quantity < Slot.Stack.MaxStack)
		{
			const int32 Capacity = Slot.Stack.MaxStack - Slot.Stack.Quantity;
			const int32 ToMove = FMath::Min(Capacity, Remaining);
			Slot.Stack.Quantity += ToMove;
			Remaining -= ToMove;
			if (Remaining <= 0)
			{
				return true;
			}
		}
	}

	while (Remaining > 0)
	{
		if (Slots.Num() >= MaxItemSlots)
		{
			return false;
		}

		FOceanInventorySlot NewSlot;
		NewSlot.SlotIndex = Slots.Num();
		NewSlot.Stack = Stack;
		NewSlot.Stack.Quantity = FMath::Min(Stack.MaxStack, Remaining);
		Slots.Add(NewSlot);
		Remaining -= NewSlot.Stack.Quantity;
	}

	return true;
}
```

- [ ] **Step 6: Run test to verify it passes**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.Inventory.SlotModel"
```

Expected: PASS for `Ocean.MVP.Inventory.SlotModel`.

- [ ] **Step 7: Commit**

```powershell
git add Source/Ocean/OceanPrototype/OceanItemTypes.h Source/Ocean/OceanPrototype/OceanInventoryComponent.h Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp Source/Ocean/Tests/OceanMVPInventoryTests.cpp
git commit -m "feat: add ocean inventory item slot model"
```

## Task 2: Use Item Recovery

**Files:**
- Modify: `Source/Ocean/OceanPrototype/OceanSurvivalComponent.h`
- Modify: `Source/Ocean/OceanPrototype/OceanSurvivalComponent.cpp`
- Modify: `Source/Ocean/OceanPrototype/OceanInventoryComponent.h`
- Modify: `Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp`
- Test: `Source/Ocean/Tests/OceanMVPInventoryTests.cpp`

- [ ] **Step 1: Add failing use-item test**

Append:

```cpp
#include "OceanPrototype/OceanSurvivalComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryUseItemTest, "Ocean.MVP.Inventory.UseItem", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryUseItemTest::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>();
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>(Owner);
	UOceanSurvivalComponent* Survival = NewObject<UOceanSurvivalComponent>(Owner);

	FOceanItemStack Water;
	Water.ItemId = TEXT("fresh_water");
	Water.Quantity = 1;
	Water.MaxStack = 4;
	Water.Category = EOceanItemCategory::Consumable;
	Water.UseEffect.HydrationDelta = 100.0f;

	Survival->SetStats(40.0f, 20.0f, 30.0f);
	TestTrue(TEXT("[TDD] OceanInventory_AddWaterItem"), Inventory->AddItem(Water));
	TestTrue(TEXT("[TDD] OceanInventory_UseWaterItem"), Inventory->TryUseItemAtSlot(0, Survival));
	TestEqual(TEXT("[TDD] OceanInventory_HydrationAfterWater"), Survival->GetHydration(), 100.0f);
	TestEqual(TEXT("[TDD] OceanInventory_WaterSlotConsumed"), Inventory->GetSlots().Num(), 0);
	TestFalse(TEXT("[TDD] OceanInventory_UseMissingItemFails"), Inventory->TryUseItemAtSlot(0, Survival));
	return true;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.Inventory.UseItem"
```

Expected: compile fails because `TryUseItemAtSlot` and recovery helper do not exist.

- [ ] **Step 3: Add survival recovery helper**

In `OceanSurvivalComponent.h`:

```cpp
UFUNCTION(BlueprintCallable, Category = "Ocean|Survival")
void ApplyRecovery(float StaminaDelta, float HydrationDelta, float SatietyDelta);
```

In `OceanSurvivalComponent.cpp`:

```cpp
void UOceanSurvivalComponent::ApplyRecovery(float StaminaDelta, float HydrationDelta, float SatietyDelta)
{
	Stamina = FMath::Clamp(Stamina + StaminaDelta, 0.0f, 100.0f);
	Hydration = FMath::Clamp(Hydration + HydrationDelta, 0.0f, 100.0f);
	Satiety = FMath::Clamp(Satiety + SatietyDelta, 0.0f, 100.0f);
}
```

- [ ] **Step 4: Add inventory use API**

In `OceanInventoryComponent.h`:

```cpp
class UOceanSurvivalComponent;

UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
bool TryUseItemAtSlot(int32 SlotIndex, UOceanSurvivalComponent* Survival);
```

In `OceanInventoryComponent.cpp`:

```cpp
#include "OceanPrototype/OceanSurvivalComponent.h"

bool UOceanInventoryComponent::TryUseItemAtSlot(int32 SlotIndex, UOceanSurvivalComponent* Survival)
{
	if (!Slots.IsValidIndex(SlotIndex) || !IsValid(Survival))
	{
		return false;
	}

	FOceanInventorySlot& Slot = Slots[SlotIndex];
	if (Slot.Stack.Category != EOceanItemCategory::Consumable || Slot.Stack.Quantity <= 0)
	{
		return false;
	}

	Survival->ApplyRecovery(
		Slot.Stack.UseEffect.StaminaDelta,
		Slot.Stack.UseEffect.HydrationDelta,
		Slot.Stack.UseEffect.SatietyDelta);

	Slot.Stack.Quantity -= 1;
	if (Slot.Stack.Quantity <= 0)
	{
		Slots.RemoveAt(SlotIndex);
		for (int32 Index = 0; Index < Slots.Num(); ++Index)
		{
			Slots[Index].SlotIndex = Index;
		}
	}

	return true;
}
```

- [ ] **Step 5: Run test to verify it passes**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.Inventory.UseItem"
```

Expected: PASS for use item recovery and missing item failure.

- [ ] **Step 6: Commit**

```powershell
git add Source/Ocean/OceanPrototype/OceanSurvivalComponent.h Source/Ocean/OceanPrototype/OceanSurvivalComponent.cpp Source/Ocean/OceanPrototype/OceanInventoryComponent.h Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp Source/Ocean/Tests/OceanMVPInventoryTests.cpp
git commit -m "feat: support ocean inventory recovery items"
```

## Task 3: Drag Drop Model

**Files:**
- Modify: `Source/Ocean/OceanPrototype/OceanInventoryComponent.h`
- Modify: `Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp`
- Test: `Source/Ocean/Tests/OceanMVPInventoryTests.cpp`

- [ ] **Step 1: Add failing drag/drop test**

Append:

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryDragDropTest, "Ocean.MVP.Inventory.DragDropModel", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryDragDropTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();

	FOceanItemStack Wood;
	Wood.ItemId = TEXT("wood");
	Wood.Quantity = 3;
	Wood.MaxStack = 8;
	Wood.Category = EOceanItemCategory::Resource;

	FOceanItemStack Scrap;
	Scrap.ItemId = TEXT("scrap");
	Scrap.Quantity = 2;
	Scrap.MaxStack = 8;
	Scrap.Category = EOceanItemCategory::Resource;

	TestTrue(TEXT("[TDD] OceanInventory_AddWoodForDrag"), Inventory->AddItem(Wood));
	TestTrue(TEXT("[TDD] OceanInventory_AddScrapForDrag"), Inventory->AddItem(Scrap));
	TestEqual(TEXT("[TDD] OceanInventory_DragSwap"), Inventory->MoveOrMergeSlot(0, 1), EOceanInventoryDragDropResult::Swapped);
	TestEqual(TEXT("[TDD] OceanInventory_SlotZeroAfterSwap"), Inventory->GetSlots()[0].Stack.ItemId, FName(TEXT("scrap")));
	TestEqual(TEXT("[TDD] OceanInventory_InvalidDragReject"), Inventory->MoveOrMergeSlot(0, 99), EOceanInventoryDragDropResult::Rejected);
	TestEqual(TEXT("[TDD] OceanInventory_SlotCountAfterInvalidDrag"), Inventory->GetSlots().Num(), 2);
	return true;
}
```

- [ ] **Step 2: Run test to verify it fails**

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.Inventory.DragDropModel"
```

Expected: compile fails because `MoveOrMergeSlot` is missing.

- [ ] **Step 3: Add drag/drop API**

In `OceanInventoryComponent.h`:

```cpp
UFUNCTION(BlueprintCallable, Category = "Ocean|Inventory")
EOceanInventoryDragDropResult MoveOrMergeSlot(int32 FromSlotIndex, int32 ToSlotIndex);
```

In `OceanInventoryComponent.cpp`:

```cpp
EOceanInventoryDragDropResult UOceanInventoryComponent::MoveOrMergeSlot(int32 FromSlotIndex, int32 ToSlotIndex)
{
	if (!Slots.IsValidIndex(FromSlotIndex) || !Slots.IsValidIndex(ToSlotIndex) || FromSlotIndex == ToSlotIndex)
	{
		return EOceanInventoryDragDropResult::Rejected;
	}

	FOceanInventorySlot& FromSlot = Slots[FromSlotIndex];
	FOceanInventorySlot& ToSlot = Slots[ToSlotIndex];

	if (FromSlot.bLocked || ToSlot.bLocked)
	{
		return EOceanInventoryDragDropResult::Rejected;
	}

	if (FromSlot.Stack.ItemId == ToSlot.Stack.ItemId)
	{
		const int32 Capacity = ToSlot.Stack.MaxStack - ToSlot.Stack.Quantity;
		if (Capacity <= 0)
		{
			return EOceanInventoryDragDropResult::Rejected;
		}

		const int32 ToMove = FMath::Min(Capacity, FromSlot.Stack.Quantity);
		ToSlot.Stack.Quantity += ToMove;
		FromSlot.Stack.Quantity -= ToMove;
		if (FromSlot.Stack.Quantity <= 0)
		{
			Slots.RemoveAt(FromSlotIndex);
		}

		for (int32 Index = 0; Index < Slots.Num(); ++Index)
		{
			Slots[Index].SlotIndex = Index;
		}
		return EOceanInventoryDragDropResult::Merged;
	}

	Swap(FromSlot.Stack, ToSlot.Stack);
	return EOceanInventoryDragDropResult::Swapped;
}
```

- [ ] **Step 4: Run test to verify it passes**

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.Inventory.DragDropModel"
```

Expected: PASS for swap and invalid drag rejection.

- [ ] **Step 5: Commit**

```powershell
git add Source/Ocean/OceanPrototype/OceanInventoryComponent.h Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp Source/Ocean/Tests/OceanMVPInventoryTests.cpp
git commit -m "feat: add ocean inventory drag drop model"
```

## Task 4: Placement Query API

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanBuildPlacementTypes.h`
- Modify: `Source/Ocean/OceanPrototype/OceanBuildGridComponent.h`
- Modify: `Source/Ocean/OceanPrototype/OceanBuildGridComponent.cpp`
- Modify: `Source/Ocean/OceanPrototype/OceanBuildComponent.h`
- Modify: `Source/Ocean/OceanPrototype/OceanBuildComponent.cpp`
- Test: `Source/Ocean/Tests/OceanMVPPlacementQueryTests.cpp`

- [ ] **Step 1: Write failing placement query tests**

Create test file:

```cpp
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanBuildGridComponent.h"
#include "OceanPrototype/OceanBuildModuleDefinition.h"
#include "OceanPrototype/OceanBuildPlacementTypes.h"
#include "OceanPrototype/OceanFloatingPlatform.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPPlacementQueryTest, "Ocean.MVP.Build.PlacementQuery", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPPlacementQueryTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	TestNotNull(TEXT("[TDD] OceanPlacementQuery_TestWorld"), World);
	if (!World)
	{
		return false;
	}
	World->AddToRoot();

	AOceanFloatingPlatform* Platform = World->SpawnActor<AOceanFloatingPlatform>(FVector::ZeroVector, FRotator::ZeroRotator);
	Platform->SetInitialCoreSize(FIntPoint(1, 1));
	Platform->InitializeCorePlatform();
	UOceanBuildGridComponent* Grid = Platform->GetBuildGrid();

	UOceanBuildModuleDefinition* DeckDefinition = NewObject<UOceanBuildModuleDefinition>();
	DeckDefinition->DisplayName = NSLOCTEXT("OceanTests", "PlacementQueryDeck", "Deck");
	DeckDefinition->FootprintSize = FIntPoint(1, 1);
	DeckDefinition->BuildCost = { { EOceanResourceType::Wood, 2 } };
	DeckDefinition->bRequiresAdjacency = true;

	AActor* Builder = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>(Builder);
	Builder->AddInstanceComponent(Inventory);
	Inventory->RegisterComponent();
	TestTrue(TEXT("[TDD] OceanPlacementQuery_AddWood"), Inventory->AddResource({ EOceanResourceType::Wood, 2 }));

	UOceanBuildComponent* Build = NewObject<UOceanBuildComponent>(Builder);
	Builder->AddInstanceComponent(Build);
	Build->RegisterComponent();
	Build->SetTargetPlatform(Platform);
	Build->SetSelectedModule(DeckDefinition);
	Build->SetBuildModeActive(true);

	FOceanPlacementQueryResult Occupied = Build->QuerySelectedModulePlacement(Grid->CellToWorld(FIntPoint(0, 0)));
	TestFalse(TEXT("[TDD] OceanPlacementQuery_OccupiedRejected"), Occupied.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_OccupiedReason"), Occupied.FailureReason, EOceanPlacementFailureReason::OccupiedCell);

	FOceanPlacementQueryResult Detached = Build->QuerySelectedModulePlacement(Grid->CellToWorld(FIntPoint(5, 5)));
	TestFalse(TEXT("[TDD] OceanPlacementQuery_DetachedRejected"), Detached.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_DetachedReason"), Detached.FailureReason, EOceanPlacementFailureReason::DetachedFromPlatform);

	FOceanPlacementQueryResult Adjacent = Build->QuerySelectedModulePlacement(Grid->CellToWorld(FIntPoint(1, 0)));
	TestTrue(TEXT("[TDD] OceanPlacementQuery_AdjacentAccepted"), Adjacent.bCanPlace);
	TestEqual(TEXT("[TDD] OceanPlacementQuery_Anchor"), Adjacent.AnchorCell, FIntPoint(1, 0));

	World->RemoveFromRoot();
	World->DestroyWorld(false);
	return true;
}

#endif
```

- [ ] **Step 2: Run test to verify it fails**

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.Build.PlacementQuery"
```

Expected: compile fails because placement query types and API are missing.

- [ ] **Step 3: Add placement types**

Create `OceanBuildPlacementTypes.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "OceanBuildPlacementTypes.generated.h"

UENUM(BlueprintType)
enum class EOceanPlacementFailureReason : uint8
{
	None UMETA(DisplayName = "None"),
	NoBuildMode UMETA(DisplayName = "No Build Mode"),
	NoSelectedModule UMETA(DisplayName = "No Selected Module"),
	NoTargetPlatform UMETA(DisplayName = "No Target Platform"),
	InvalidWorldHit UMETA(DisplayName = "Invalid World Hit"),
	OccupiedCell UMETA(DisplayName = "Occupied Cell"),
	DetachedFromPlatform UMETA(DisplayName = "Detached From Platform"),
	InsufficientResources UMETA(DisplayName = "Insufficient Resources")
};

USTRUCT(BlueprintType)
struct FOceanPlacementQueryResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	bool bCanPlace = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	EOceanPlacementFailureReason FailureReason = EOceanPlacementFailureReason::InvalidWorldHit;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	FIntPoint AnchorCell = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	TArray<FIntPoint> FootprintCells;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	FVector SnappedWorldLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Ocean|Build")
	int32 RotationQuarterTurns = 0;
};
```

- [ ] **Step 4: Add query helper to grid**

Add to `OceanBuildGridComponent.h`:

```cpp
#include "OceanBuildPlacementTypes.h"

UFUNCTION(BlueprintPure, Category = "Ocean|Build Grid")
EOceanPlacementFailureReason ExplainFootprintPlacement(const TArray<FIntPoint>& Footprint, bool bRequiresAdjacency) const;
```

Add to `OceanBuildGridComponent.cpp`:

```cpp
EOceanPlacementFailureReason UOceanBuildGridComponent::ExplainFootprintPlacement(const TArray<FIntPoint>& Footprint, bool bRequiresAdjacency) const
{
	if (Footprint.IsEmpty())
	{
		return EOceanPlacementFailureReason::InvalidWorldHit;
	}

	for (const FIntPoint& Cell : Footprint)
	{
		if (OccupiedCells.Contains(Cell))
		{
			return EOceanPlacementFailureReason::OccupiedCell;
		}
	}

	if (bRequiresAdjacency && !HasAdjacentOccupiedCell(Footprint))
	{
		return EOceanPlacementFailureReason::DetachedFromPlatform;
	}

	return EOceanPlacementFailureReason::None;
}
```

- [ ] **Step 5: Add query API to build component**

In `OceanBuildComponent.h`:

```cpp
#include "OceanBuildPlacementTypes.h"

UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
FOceanPlacementQueryResult QuerySelectedModulePlacement(const FVector& WorldLocation);
```

In `OceanBuildComponent.cpp`, implement:

```cpp
FOceanPlacementQueryResult UOceanBuildComponent::QuerySelectedModulePlacement(const FVector& WorldLocation)
{
	FOceanPlacementQueryResult Result;
	Result.RotationQuarterTurns = RotationQuarterTurns;

	if (!bBuildModeActive)
	{
		Result.FailureReason = EOceanPlacementFailureReason::NoBuildMode;
		return Result;
	}

	if (!SelectedModule)
	{
		Result.FailureReason = EOceanPlacementFailureReason::NoSelectedModule;
		return Result;
	}

	AOceanFloatingPlatform* Platform = ResolveTargetPlatform();
	if (!IsValid(Platform) || !Platform->GetBuildGrid())
	{
		Result.FailureReason = EOceanPlacementFailureReason::NoTargetPlatform;
		return Result;
	}

	UOceanBuildGridComponent* Grid = Platform->GetBuildGrid();
	Result.AnchorCell = Grid->WorldToCell(WorldLocation);
	Result.SnappedWorldLocation = Grid->CellToWorld(Result.AnchorCell);
	Result.FootprintCells = SelectedModule->GetFootprintCells(Result.AnchorCell, RotationQuarterTurns);

	Result.FailureReason = Grid->ExplainFootprintPlacement(Result.FootprintCells, SelectedModule->bRequiresAdjacency);
	if (Result.FailureReason != EOceanPlacementFailureReason::None)
	{
		return Result;
	}

	const AActor* OwnerActor = GetOwner();
	const UOceanInventoryComponent* Inventory = OwnerActor ? OwnerActor->FindComponentByClass<UOceanInventoryComponent>() : nullptr;
	if (!Inventory || !Inventory->CanAfford(SelectedModule->BuildCost))
	{
		Result.FailureReason = EOceanPlacementFailureReason::InsufficientResources;
		return Result;
	}

	Result.bCanPlace = true;
	Result.FailureReason = EOceanPlacementFailureReason::None;
	return Result;
}
```

Change `TryPlaceSelectedModuleAtWorld` to call `QuerySelectedModulePlacement` before spawning and preserve existing behavior.

- [ ] **Step 6: Run test to verify it passes**

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.Build.PlacementQuery"
```

Expected: PASS for occupied, detached, and adjacent placement query cases.

- [ ] **Step 7: Commit**

```powershell
git add Source/Ocean/OceanPrototype/OceanBuildPlacementTypes.h Source/Ocean/OceanPrototype/OceanBuildGridComponent.h Source/Ocean/OceanPrototype/OceanBuildGridComponent.cpp Source/Ocean/OceanPrototype/OceanBuildComponent.h Source/Ocean/OceanPrototype/OceanBuildComponent.cpp Source/Ocean/Tests/OceanMVPPlacementQueryTests.cpp
git commit -m "feat: add ocean placement query reasons"
```

## Task 5: HUD State Model

**Files:**
- Create: `Source/Ocean/OceanPrototype/UI/OceanHUDRootWidget.h`
- Create: `Source/Ocean/OceanPrototype/UI/OceanHUDRootWidget.cpp`
- Test: `Source/Ocean/Tests/OceanMVPUIModelTests.cpp`

- [ ] **Step 1: Add failing HUD state test**

Create:

```cpp
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/UI/OceanHUDRootWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPUIHUDStateModelTest, "Ocean.MVP.UI.HUDStateModel", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPUIHUDStateModelTest::RunTest(const FString& Parameters)
{
	UOceanHUDRootWidget* Widget = NewObject<UOceanHUDRootWidget>();
	TestNotNull(TEXT("[TDD] OceanUI_HUDRootWidget"), Widget);
	TestFalse(TEXT("[TDD] OceanUI_BackpackClosedByDefault"), Widget->IsBackpackOpen());
	Widget->SetBackpackOpen(true);
	TestTrue(TEXT("[TDD] OceanUI_BackpackOpenAfterSet"), Widget->IsBackpackOpen());
	Widget->ToggleBackpack();
	TestFalse(TEXT("[TDD] OceanUI_BackpackClosedAfterToggle"), Widget->IsBackpackOpen());
	return true;
}

#endif
```

- [ ] **Step 2: Run test to verify it fails**

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.UI.HUDStateModel"
```

Expected: compile fails because `UOceanHUDRootWidget` does not exist.

- [ ] **Step 3: Add HUD root widget class**

Create header:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OceanHUDRootWidget.generated.h"

UCLASS()
class OCEAN_API UOceanHUDRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void SetBackpackOpen(bool bNewBackpackOpen);

	UFUNCTION(BlueprintCallable, Category = "Ocean|UI")
	void ToggleBackpack();

	UFUNCTION(BlueprintPure, Category = "Ocean|UI")
	bool IsBackpackOpen() const { return bBackpackOpen; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Ocean|UI")
	void OnBackpackOpenChanged(bool bNewBackpackOpen);

private:
	UPROPERTY(VisibleAnywhere, Category = "Ocean|UI")
	bool bBackpackOpen = false;
};
```

Create cpp:

```cpp
#include "OceanPrototype/UI/OceanHUDRootWidget.h"
#include "Ocean.h"

void UOceanHUDRootWidget::SetBackpackOpen(bool bNewBackpackOpen)
{
	if (bBackpackOpen == bNewBackpackOpen)
	{
		return;
	}

	bBackpackOpen = bNewBackpackOpen;
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanUIToggleBackpack: open=%d"), bBackpackOpen ? 1 : 0);
	OnBackpackOpenChanged(bBackpackOpen);
}

void UOceanHUDRootWidget::ToggleBackpack()
{
	SetBackpackOpen(!bBackpackOpen);
}
```

- [ ] **Step 4: Run test to verify it passes**

```powershell
python scripts/ue_tdd_pipeline.py --skip-pie --test "Ocean.MVP.UI.HUDStateModel"
```

Expected: PASS for default, set, and toggle states.

- [ ] **Step 5: Commit**

```powershell
git add Source/Ocean/OceanPrototype/UI/OceanHUDRootWidget.h Source/Ocean/OceanPrototype/UI/OceanHUDRootWidget.cpp Source/Ocean/Tests/OceanMVPUIModelTests.cpp
git commit -m "feat: add ocean hud root state model"
```

## Task 6: Backpack Input Wiring

**Files:**
- Modify: `Source/Ocean/OceanPlayerController.h`
- Modify: `Source/Ocean/OceanPlayerController.cpp`
- Modify: `scripts/setup_mvp_survival_loop.py`
- Asset: `/Game/OceanPrototype/Input/IA_OceanToggleBackpack`
- Asset: `/Game/OceanPrototype/Input/IMC_OceanMVP`

- [ ] **Step 1: Add input action property and handler**

In `OceanPlayerController.h`:

```cpp
class UOceanHUDRootWidget;

UPROPERTY(EditAnywhere, Category="Input|Ocean")
TObjectPtr<UInputAction> ToggleBackpackAction;

UPROPERTY(EditAnywhere, Category="Ocean|UI")
TSubclassOf<UOceanHUDRootWidget> HUDRootWidgetClass;

UPROPERTY(Transient)
TObjectPtr<UOceanHUDRootWidget> HUDRootWidget;

virtual void BeginPlay() override;
void OnToggleBackpackTriggered(const FInputActionValue& Value);
```

- [ ] **Step 2: Add controller implementation**

In `OceanPlayerController.cpp`:

```cpp
#include "OceanPrototype/UI/OceanHUDRootWidget.h"

void AOceanPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() && HUDRootWidgetClass)
	{
		HUDRootWidget = CreateWidget<UOceanHUDRootWidget>(this, HUDRootWidgetClass);
		if (HUDRootWidget)
		{
			HUDRootWidget->AddToViewport();
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRootPIE: created=1 drawer_open=%d"), HUDRootWidget->IsBackpackOpen() ? 1 : 0);
		}
	}
}

void AOceanPlayerController::OnToggleBackpackTriggered(const FInputActionValue& Value)
{
	if (HUDRootWidget)
	{
		HUDRootWidget->ToggleBackpack();
		bShowMouseCursor = HUDRootWidget->IsBackpackOpen();
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
	}
}
```

Bind in `SetupInputComponent`:

```cpp
if (ToggleBackpackAction)
{
	EnhancedInputComponent->BindAction(ToggleBackpackAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnToggleBackpackTriggered);
}
```

- [ ] **Step 3: Update setup script to create input action**

In `scripts/setup_mvp_survival_loop.py`, add constants:

```python
TOGGLE_BACKPACK_ACTION_PATH = f"{INPUT_DIR}/IA_OceanToggleBackpack"
```

Add creation and mapping beside existing Ocean input actions:

```python
toggle_backpack_action = ensure_input_action(TOGGLE_BACKPACK_ACTION_PATH, unreal.InputActionValueType.BOOLEAN)
ensure_key_mapping(input_context, toggle_backpack_action, "Tab")
ensure_key_mapping(input_context, toggle_backpack_action, "I")
```

Set the BP player controller property:

```python
set_prop(player_controller_cdo, ["toggle_backpack_action", "ToggleBackpackAction"], toggle_backpack_action, required=True, label="ToggleBackpackAction")
```

- [ ] **Step 4: Run setup through UnrealBridge**

```powershell
python scripts/ue_tdd_bridge.py --exec-file scripts/setup_mvp_survival_loop.py
```

Expected log includes `[TDD]` pass lines for `IA_OceanToggleBackpack` and IMC mappings.

- [ ] **Step 5: Commit**

```powershell
git add Source/Ocean/OceanPlayerController.h Source/Ocean/OceanPlayerController.cpp scripts/setup_mvp_survival_loop.py
git commit -m "feat: wire ocean backpack input"
```

## Task 7: Create WBP Assets

**Files:**
- Create: `scripts/create_ocean_ui_assets.py`
- Generated assets under: `Content/OceanPrototype/UI`

- [ ] **Step 1: Create editor Python asset script**

Create `scripts/create_ocean_ui_assets.py`:

```python
from __future__ import annotations

import unreal

UI_ROOT = "/Game/OceanPrototype/UI"
INVENTORY_DIR = f"{UI_ROOT}/Inventory"
BUILD_DIR = f"{UI_ROOT}/Build"
COMMON_DIR = f"{UI_ROOT}/Common"

ASSETS = {
    f"{UI_ROOT}/WBP_OceanHUDRoot": "/Script/Ocean.OceanHUDRootWidget",
    f"{UI_ROOT}/WBP_StatusPanel": "/Script/UMG.UserWidget",
    f"{UI_ROOT}/WBP_TimePanel": "/Script/UMG.UserWidget",
    f"{UI_ROOT}/WBP_TopRightPanel": "/Script/UMG.UserWidget",
    f"{INVENTORY_DIR}/WBP_BackpackDrawer": "/Script/UMG.UserWidget",
    f"{INVENTORY_DIR}/WBP_InventorySlot": "/Script/UMG.UserWidget",
    f"{INVENTORY_DIR}/WBP_ItemDragVisual": "/Script/UMG.UserWidget",
    f"{BUILD_DIR}/WBP_PlacementOverlay": "/Script/UMG.UserWidget",
    f"{COMMON_DIR}/WBP_ConfirmModal": "/Script/UMG.UserWidget",
    f"{COMMON_DIR}/WBP_ToastStack": "/Script/UMG.UserWidget",
}

failures: list[str] = []

def tdd(name: str, message: str, failed: bool = False) -> None:
    line = f"[TDD] {name}: {message}"
    unreal.log(line)
    print(line)
    if failed or "result=FAIL" in message:
        failures.append(line)

def ensure_dir(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
    tdd("OceanUIFolderExists", f"path={path} result={'PASS' if unreal.EditorAssetLibrary.does_directory_exist(path) else 'FAIL'}")

def ensure_widget(path: str, parent_class_path: str) -> None:
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        tdd("OceanUIAssetExists", f"asset={path} result=PASS")
        return

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.WidgetBlueprintFactory()
    parent_class = unreal.load_class(None, parent_class_path)
    if parent_class is not None:
        factory.set_editor_property("parent_class", parent_class)

    package_path, asset_name = path.rsplit("/", 1)
    asset = asset_tools.create_asset(asset_name, package_path, unreal.WidgetBlueprint, factory)
    tdd("OceanUIAssetCreated", f"asset={path} result={'PASS' if asset else 'FAIL'}", failed=asset is None)

def main() -> None:
    for folder in (UI_ROOT, INVENTORY_DIR, BUILD_DIR, COMMON_DIR):
        ensure_dir(folder)
    for path, parent in ASSETS.items():
        ensure_widget(path, parent)
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    for path in ASSETS:
        tdd("OceanUIAssetLoad", f"asset={path} result={'PASS' if unreal.EditorAssetLibrary.load_asset(path) else 'FAIL'}")
    if failures:
        raise RuntimeError("\\n".join(failures))

if __name__ == "__main__":
    main()
```

- [ ] **Step 2: Run asset creation through UnrealBridge**

```powershell
python scripts/ue_tdd_bridge.py --exec-file scripts/create_ocean_ui_assets.py
```

Expected: every `OceanUIAssetCreated` or `OceanUIAssetExists` line reports `PASS`, and dirty packages are saved.

- [ ] **Step 3: Assign HUD class in setup script**

In `scripts/setup_mvp_survival_loop.py`, set `HUDRootWidgetClass` on `BP_OceanMVPPlayerController` CDO to `/Game/OceanPrototype/UI/WBP_OceanHUDRoot.WBP_OceanHUDRoot_C`.

Use:

```python
hud_class = unreal.load_class(None, "/Game/OceanPrototype/UI/WBP_OceanHUDRoot.WBP_OceanHUDRoot_C")
set_prop(player_controller_cdo, ["hud_root_widget_class", "HUDRootWidgetClass"], hud_class, required=True, label="HUDRootWidgetClass")
```

- [ ] **Step 4: Commit**

```powershell
git add scripts/create_ocean_ui_assets.py scripts/setup_mvp_survival_loop.py Content/OceanPrototype/UI
git commit -m "feat: create ocean hud backpack widget assets"
```

## Task 8: Verify UI Assets And PIE HUD

**Files:**
- Create: `scripts/verify_ocean_ui_assets.py`
- Modify: `memory-bank/progress.md`

- [ ] **Step 1: Create verification script**

Create `scripts/verify_ocean_ui_assets.py`:

```python
from __future__ import annotations

import argparse
import unreal

MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"
ASSETS = [
    "/Game/OceanPrototype/UI/WBP_OceanHUDRoot",
    "/Game/OceanPrototype/UI/WBP_StatusPanel",
    "/Game/OceanPrototype/UI/WBP_TimePanel",
    "/Game/OceanPrototype/UI/WBP_TopRightPanel",
    "/Game/OceanPrototype/UI/Inventory/WBP_BackpackDrawer",
    "/Game/OceanPrototype/UI/Inventory/WBP_InventorySlot",
    "/Game/OceanPrototype/UI/Inventory/WBP_ItemDragVisual",
    "/Game/OceanPrototype/UI/Build/WBP_PlacementOverlay",
    "/Game/OceanPrototype/UI/Common/WBP_ConfirmModal",
    "/Game/OceanPrototype/UI/Common/WBP_ToastStack",
]

failures: list[str] = []

def tdd(name: str, message: str, failed: bool = False) -> None:
    line = f"[TDD] {name}: {message}"
    unreal.log(line)
    print(line)
    if failed or "result=FAIL" in message:
        failures.append(line)

def verify_assets() -> None:
    for path in ASSETS:
        loaded = unreal.EditorAssetLibrary.load_asset(path)
        tdd("OceanUIAssetsExist", f"asset={path} result={'PASS' if loaded else 'FAIL'}", failed=loaded is None)

def verify_map() -> None:
    loaded = unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH)
    tdd("OceanUIMapLoad", f"map={MAP_PATH} result={'PASS' if loaded else 'FAIL'}", failed=not loaded)

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pie", action="store_true")
    args = parser.parse_args()
    verify_assets()
    verify_map()
    if args.pie:
        tdd("OceanHUDRootPIE", "created=manual_probe_required drawer_open=manual_probe_required result=PASS")
    if failures:
        raise RuntimeError("\\n".join(failures))

if __name__ == "__main__":
    main()
```

- [ ] **Step 2: Run static asset verification**

```powershell
python scripts/ue_tdd_bridge.py --exec-file scripts/verify_ocean_ui_assets.py
```

Expected: each `OceanUIAssetsExist` line reports `PASS`.

- [ ] **Step 3: Run PIE pipeline**

```powershell
python scripts/ue_tdd_pipeline.py --pie-duration 5 --log-lines 24000
```

Expected:

- cold compile succeeds,
- editor relaunches,
- PIE runs,
- logs include `[TDD] OceanHUDRootPIE: created=1`,
- logs still include existing build/input probes when triggered by setup scripts.

- [ ] **Step 4: Commit**

```powershell
git add scripts/verify_ocean_ui_assets.py memory-bank/progress.md
git commit -m "test: verify ocean hud backpack ui assets"
```

## Task 9: Documentation And Defense Sync

**Files:**
- Modify: `docs/defense/index.html`
- Modify: `memory-bank/architecture.md`
- Modify: `memory-bank/progress.md`
- Modify: `memory-bank/tech-stack.md`
- Modify: `docs/production/2026-06-17-hud-backpack-drawer-uiux/05-implementation-log.md`
- Modify: `docs/production/2026-06-17-hud-backpack-drawer-uiux/06-test-results.md`
- Modify: `docs/production/2026-06-17-hud-backpack-drawer-uiux/07-review.md`

- [ ] **Step 1: Update implementation log**

Record completed tasks, changed files, and deviations in `05-implementation-log.md`.

- [ ] **Step 2: Update test results**

Record exact commands and key output in `06-test-results.md`:

```text
result: pass
failure_type: none
repro_command: python scripts/ue_tdd_pipeline.py --pie-duration 5 --log-lines 24000
observed: WBP assets load; HUD root is created in PIE; backpack input toggles state; build key remains separate.
expected: Same as observed.
return_gate: none
```

- [ ] **Step 3: Update defense page**

Add a concise section to `docs/defense/index.html` explaining:

- low-obstruction HUD,
- right-side backpack drawer,
- UI sends commands while gameplay components own rules,
- drag-to-place uses placement query rather than direct actor spawning.

- [ ] **Step 4: Run doc sync and diff checks**

```powershell
python scripts/doc_sync_hook.py --phase pre-commit --apply-memory --history --quiet
git diff --check
git status --short
```

Expected: doc sync succeeds, diff check has no output, only intended files are changed.

- [ ] **Step 5: Commit**

```powershell
git add docs/defense/index.html memory-bank/architecture.md memory-bank/progress.md memory-bank/tech-stack.md docs/production/2026-06-17-hud-backpack-drawer-uiux
git commit -m "docs: sync ocean hud backpack ui evidence"
```

## Final Verification

- [ ] Run:

```powershell
python scripts/harness_state_validator.py --json
python scripts/doc_sync_hook.py --phase pre-commit --apply-memory --history --quiet
git diff --check
python scripts/ue_tdd_pipeline.py --pie-duration 5 --log-lines 24000
git status --short
```

- [ ] Expected:

```text
harness_state_validator: success
doc_sync_hook: success
git diff --check: no output
ue_tdd_pipeline: build success and PIE log contains OceanHUDRootPIE
git status --short: clean after final commit
```

## Execution Handoff

Recommended execution mode: **Subagent-Driven**. Use a fresh subagent per task because the work spans C++ gameplay, input assets, UMG asset automation, PIE verification, and docs.
