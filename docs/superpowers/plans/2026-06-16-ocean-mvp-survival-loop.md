# Ocean MVP Survival Loop Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the first playable 《洋流》 loop on `L_WaterOcean`: click-to-move plus WASD, `F` pickup interaction, three survival stats, stack inventory, visible HUD, floating resources, and one deck expansion module.

**Architecture:** Keep reusable gameplay in focused C++ components under `Source/Ocean/OceanPrototype`; keep the existing TopDown click-move controller and extend it instead of replacing it. Use editor Python only to create/wire assets and map actors after C++ compiles. Existing build-grid, module, resource, Water, PCG, and UnrealBridge infrastructure remain the foundation.

**Tech Stack:** Unreal Engine 5.7, C++, Enhanced Input, UMG/HUD debug drawing, Water `UBuoyancyComponent`, PCG/fallback resource spawning, UnrealBridge, no-LiveCoding cold build pipeline.

---

## Scope Check

This plan implements the frozen MVP only:

- In scope: movement, interaction, pickup, survival stats, inventory, HUD, one deck build path, setup/verification scripts.
- Deferred from this plan: fishing, diving, island exploration, playable cruise intro, save/load, complex inventory UI, final art.

## File Structure

### Runtime C++ files

- Create: `Source/Ocean/OceanPrototype/OceanInputMath.h`
- Create: `Source/Ocean/OceanPrototype/OceanInputMath.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanInventoryComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanSurvivalComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanSurvivalComponent.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanInteractableInterface.h`
- Create: `Source/Ocean/OceanPrototype/OceanInteractionComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanInteractionComponent.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanBuildComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanBuildComponent.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanSurvivalHUD.h`
- Create: `Source/Ocean/OceanPrototype/OceanSurvivalHUD.cpp`
- Modify: `Source/Ocean/OceanPrototype/OceanResourceTypes.h`
- Modify: `Source/Ocean/OceanPrototype/OceanResourceNode.h`
- Modify: `Source/Ocean/OceanPrototype/OceanResourceNode.cpp`
- Modify: `Source/Ocean/OceanCharacter.h`
- Modify: `Source/Ocean/OceanCharacter.cpp`
- Modify: `Source/Ocean/OceanPlayerController.h`
- Modify: `Source/Ocean/OceanPlayerController.cpp`
- Modify: `Source/Ocean/OceanGameMode.cpp`

### Tests

- Create: `Source/Ocean/Tests/OceanMVPInputTests.cpp`
- Create: `Source/Ocean/Tests/OceanMVPSurvivalTests.cpp`
- Create: `Source/Ocean/Tests/OceanMVPInteractionTests.cpp`
- Create: `Source/Ocean/Tests/OceanMVPBuildTests.cpp`

### Editor automation

- Create: `scripts/setup_mvp_survival_loop.py`
- Create: `scripts/verify_mvp_survival_loop.py`
- Modify: `memory-bank/architecture.md`
- Modify: `memory-bank/progress.md`
- Modify: `docs/defense/commit-log.md`

### Generated assets

The setup script creates or updates:

- `/Game/OceanPrototype/Input/IA_OceanMove`
- `/Game/OceanPrototype/Input/IA_OceanInteract`
- `/Game/OceanPrototype/Input/IA_OceanToggleBuild`
- `/Game/OceanPrototype/Input/IA_OceanRotateBuild`
- `/Game/OceanPrototype/Input/IMC_OceanMVP`
- `/Game/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter`
- `/Game/OceanPrototype/Blueprints/BP_OceanMVPGameMode`
- `/Game/OceanPrototype/Build/DA_BuildModule_Deck_1x1`

---

## Task 1: Input Math Test And WASD Extension

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanInputMath.h`
- Create: `Source/Ocean/OceanPrototype/OceanInputMath.cpp`
- Create: `Source/Ocean/Tests/OceanMVPInputTests.cpp`
- Modify: `Source/Ocean/OceanPlayerController.h`
- Modify: `Source/Ocean/OceanPlayerController.cpp`

- [ ] **Step 1: Write the failing input math test**

Create `Source/Ocean/Tests/OceanMVPInputTests.cpp`:

```cpp
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInputMath.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPCameraRelativeMoveTest, "Ocean.MVP.Input.CameraRelativeMove", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPCameraRelativeMoveTest::RunTest(const FString& Parameters)
{
	const FRotator CameraYaw(0.0f, 45.0f, 0.0f);

	const FVector Forward = FOceanInputMath::MakeCameraRelativeMoveDirection(CameraYaw, FVector2D(0.0f, 1.0f));
	TestTrue(TEXT("W produces a normalized direction"), Forward.IsNearlyZero() == false);
	TestEqual(TEXT("W follows flattened camera forward X"), FMath::RoundToInt(Forward.X * 100.0f), 71);
	TestEqual(TEXT("W follows flattened camera forward Y"), FMath::RoundToInt(Forward.Y * 100.0f), 71);

	const FVector Right = FOceanInputMath::MakeCameraRelativeMoveDirection(CameraYaw, FVector2D(1.0f, 0.0f));
	TestEqual(TEXT("D follows flattened camera right X"), FMath::RoundToInt(Right.X * 100.0f), -71);
	TestEqual(TEXT("D follows flattened camera right Y"), FMath::RoundToInt(Right.Y * 100.0f), 71);

	const FVector Diagonal = FOceanInputMath::MakeCameraRelativeMoveDirection(CameraYaw, FVector2D(1.0f, 1.0f));
	TestTrue(TEXT("Diagonal movement is clamped to unit length"), Diagonal.Size() <= 1.001f);

	return true;
}

#endif
```

- [ ] **Step 2: Run the cold build to verify the test fails**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --build-only
```

Expected: build fails because `OceanPrototype/OceanInputMath.h` does not exist.

- [ ] **Step 3: Implement the input math helper**

Create `Source/Ocean/OceanPrototype/OceanInputMath.h`:

```cpp
#pragma once

#include "CoreMinimal.h"

struct FOceanInputMath
{
	static FVector MakeCameraRelativeMoveDirection(const FRotator& ViewRotation, const FVector2D& InputVector);
};
```

Create `Source/Ocean/OceanPrototype/OceanInputMath.cpp`:

```cpp
#include "OceanPrototype/OceanInputMath.h"

FVector FOceanInputMath::MakeCameraRelativeMoveDirection(const FRotator& ViewRotation, const FVector2D& InputVector)
{
	if (InputVector.IsNearlyZero())
	{
		return FVector::ZeroVector;
	}

	const FRotator YawRotation(0.0f, ViewRotation.Yaw, 0.0f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	return (Forward * InputVector.Y + Right * InputVector.X).GetClampedToMaxSize(1.0f);
}
```

- [ ] **Step 4: Extend the player controller input API**

Modify `Source/Ocean/OceanPlayerController.h` by adding:

```cpp
class UInputAction;
struct FInputActionValue;

UPROPERTY(EditAnywhere, Category="Input|Ocean")
TObjectPtr<UInputAction> MoveAction;

UPROPERTY(EditAnywhere, Category="Input|Ocean")
TObjectPtr<UInputAction> InteractAction;

UPROPERTY(EditAnywhere, Category="Input|Ocean")
TObjectPtr<UInputAction> ToggleBuildAction;

UPROPERTY(EditAnywhere, Category="Input|Ocean")
TObjectPtr<UInputAction> RotateBuildAction;

void OnMoveTriggered(const FInputActionValue& Value);
void OnMoveCompleted(const FInputActionValue& Value);
void OnInteractTriggered(const FInputActionValue& Value);
void OnToggleBuildTriggered(const FInputActionValue& Value);
void OnRotateBuildTriggered(const FInputActionValue& Value);
```

Modify `Source/Ocean/OceanPlayerController.cpp` to bind actions only when assets are assigned:

```cpp
if (MoveAction)
{
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOceanPlayerController::OnMoveTriggered);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AOceanPlayerController::OnMoveCompleted);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AOceanPlayerController::OnMoveCompleted);
}

if (InteractAction)
{
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnInteractTriggered);
}

if (ToggleBuildAction)
{
	EnhancedInputComponent->BindAction(ToggleBuildAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnToggleBuildTriggered);
}

if (RotateBuildAction)
{
	EnhancedInputComponent->BindAction(RotateBuildAction, ETriggerEvent::Started, this, &AOceanPlayerController::OnRotateBuildTriggered);
}
```

Add movement handling:

```cpp
void AOceanPlayerController::OnMoveTriggered(const FInputActionValue& Value)
{
	StopMovement();

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	const FVector2D InputVector = Value.Get<FVector2D>();
	FRotator ViewRotation = GetControlRotation();
	FVector ViewLocation = FVector::ZeroVector;
	GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector WorldDirection = FOceanInputMath::MakeCameraRelativeMoveDirection(ViewRotation, InputVector);
	if (!WorldDirection.IsNearlyZero())
	{
		ControlledPawn->AddMovementInput(WorldDirection, 1.0f, false);
	}
}

void AOceanPlayerController::OnMoveCompleted(const FInputActionValue& Value)
{
}
```

Add temporary no-op handlers for non-movement actions so compilation succeeds in this task:

```cpp
void AOceanPlayerController::OnInteractTriggered(const FInputActionValue& Value)
{
}

void AOceanPlayerController::OnToggleBuildTriggered(const FInputActionValue& Value)
{
}

void AOceanPlayerController::OnRotateBuildTriggered(const FInputActionValue& Value)
{
}
```

- [ ] **Step 5: Run build and input test**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --build-only
```

Expected: build exits with code `0`.

- [ ] **Step 6: Commit input extension**

Run:

```powershell
git add Source/Ocean/OceanPrototype/OceanInputMath.* Source/Ocean/Tests/OceanMVPInputTests.cpp Source/Ocean/OceanPlayerController.*
git commit -m "feat: add Ocean WASD input path"
```

---

## Task 2: Inventory And Survival Components

**Files:**
- Modify: `Source/Ocean/OceanPrototype/OceanResourceTypes.h`
- Create: `Source/Ocean/OceanPrototype/OceanInventoryComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanSurvivalComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanSurvivalComponent.cpp`
- Create: `Source/Ocean/Tests/OceanMVPSurvivalTests.cpp`
- Modify: `Source/Ocean/OceanCharacter.h`
- Modify: `Source/Ocean/OceanCharacter.cpp`

- [ ] **Step 1: Write failing inventory and survival tests**

Create `Source/Ocean/Tests/OceanMVPSurvivalTests.cpp`:

```cpp
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPInventoryStackingTest, "Ocean.MVP.Inventory.Stacking", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPInventoryStackingTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	Inventory->SetMaxSlots(2);

	TestTrue(TEXT("Add wood succeeds"), Inventory->AddResource({EOceanResourceType::Wood, 4}));
	TestTrue(TEXT("Add more wood stacks"), Inventory->AddResource({EOceanResourceType::Wood, 3}));
	TestEqual(TEXT("Wood amount stacked"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 7);

	TestTrue(TEXT("Add water uses second slot"), Inventory->AddResource({EOceanResourceType::Water, 1}));
	TestFalse(TEXT("Third type rejected when full"), Inventory->AddResource({EOceanResourceType::Food, 1}));

	const TArray<FOceanResourceStack> Cost = {{EOceanResourceType::Wood, 5}};
	TestTrue(TEXT("Can afford wood cost"), Inventory->CanAfford(Cost));
	TestTrue(TEXT("Spend wood succeeds"), Inventory->TrySpend(Cost));
	TestEqual(TEXT("Wood after spend"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPSurvivalTickTest, "Ocean.MVP.Survival.Tick", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPSurvivalTickTest::RunTest(const FString& Parameters)
{
	UOceanSurvivalComponent* Survival = NewObject<UOceanSurvivalComponent>();
	Survival->SetHydrationDrainPerSecond(2.0f);
	Survival->SetSatietyDrainPerSecond(3.0f);
	Survival->SetStaminaRecoverPerSecond(5.0f);
	Survival->SetStats(50.0f, 80.0f, 70.0f);

	Survival->ApplySurvivalDelta(10.0f);
	TestEqual(TEXT("Stamina recovers and clamps"), Survival->GetStamina(), 100.0f);
	TestEqual(TEXT("Hydration drains"), Survival->GetHydration(), 60.0f);
	TestEqual(TEXT("Satiety drains"), Survival->GetSatiety(), 40.0f);

	return true;
}

#endif
```

- [ ] **Step 2: Build to verify failure**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --build-only
```

Expected: build fails because `UOceanInventoryComponent` and `UOceanSurvivalComponent` are missing.

- [ ] **Step 3: Expand resource enum**

Modify `Source/Ocean/OceanPrototype/OceanResourceTypes.h`:

```cpp
UENUM(BlueprintType)
enum class EOceanResourceType : uint8
{
	Wood UMETA(DisplayName = "Wood"),
	Scrap UMETA(DisplayName = "Scrap"),
	Food UMETA(DisplayName = "Food"),
	Water UMETA(DisplayName = "Water")
};
```

- [ ] **Step 4: Implement inventory component**

Create `Source/Ocean/OceanPrototype/OceanInventoryComponent.h` with:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanResourceTypes.h"
#include "OceanInventoryComponent.generated.h"

UCLASS(ClassGroup=(Ocean), meta=(BlueprintSpawnableComponent))
class OCEAN_API UOceanInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanInventoryComponent();

	UFUNCTION(BlueprintCallable, Category="Ocean|Inventory")
	bool AddResource(FOceanResourceStack Stack);

	UFUNCTION(BlueprintCallable, Category="Ocean|Inventory")
	bool TrySpend(const TArray<FOceanResourceStack>& Cost);

	UFUNCTION(BlueprintPure, Category="Ocean|Inventory")
	bool CanAfford(const TArray<FOceanResourceStack>& Cost) const;

	UFUNCTION(BlueprintPure, Category="Ocean|Inventory")
	int32 GetResourceAmount(EOceanResourceType ResourceType) const;

	UFUNCTION(BlueprintCallable, Category="Ocean|Inventory")
	void SetMaxSlots(int32 NewMaxSlots);

	UFUNCTION(BlueprintPure, Category="Ocean|Inventory")
	const TArray<FOceanResourceStack>& GetStacks() const { return Stacks; }

private:
	UPROPERTY(EditAnywhere, Category="Ocean|Inventory", meta=(ClampMin="1"))
	int32 MaxSlots = 8;

	UPROPERTY(VisibleAnywhere, Category="Ocean|Inventory")
	TArray<FOceanResourceStack> Stacks;

	FOceanResourceStack* FindMutableStack(EOceanResourceType ResourceType);
	const FOceanResourceStack* FindStack(EOceanResourceType ResourceType) const;
};
```

Create `Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp` with minimal stack logic:

```cpp
#include "OceanPrototype/OceanInventoryComponent.h"

UOceanInventoryComponent::UOceanInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UOceanInventoryComponent::AddResource(FOceanResourceStack Stack)
{
	if (Stack.Amount <= 0)
	{
		return false;
	}

	if (FOceanResourceStack* Existing = FindMutableStack(Stack.ResourceType))
	{
		Existing->Amount += Stack.Amount;
		return true;
	}

	if (Stacks.Num() >= MaxSlots)
	{
		return false;
	}

	Stacks.Add(Stack);
	return true;
}

bool UOceanInventoryComponent::CanAfford(const TArray<FOceanResourceStack>& Cost) const
{
	for (const FOceanResourceStack& Stack : Cost)
	{
		if (Stack.Amount > 0 && GetResourceAmount(Stack.ResourceType) < Stack.Amount)
		{
			return false;
		}
	}
	return true;
}

bool UOceanInventoryComponent::TrySpend(const TArray<FOceanResourceStack>& Cost)
{
	if (!CanAfford(Cost))
	{
		return false;
	}

	for (const FOceanResourceStack& CostStack : Cost)
	{
		if (FOceanResourceStack* Existing = FindMutableStack(CostStack.ResourceType))
		{
			Existing->Amount -= CostStack.Amount;
		}
	}

	Stacks.RemoveAll([](const FOceanResourceStack& Stack) { return Stack.Amount <= 0; });
	return true;
}

int32 UOceanInventoryComponent::GetResourceAmount(EOceanResourceType ResourceType) const
{
	if (const FOceanResourceStack* Existing = FindStack(ResourceType))
	{
		return Existing->Amount;
	}
	return 0;
}

void UOceanInventoryComponent::SetMaxSlots(int32 NewMaxSlots)
{
	MaxSlots = FMath::Max(1, NewMaxSlots);
}

FOceanResourceStack* UOceanInventoryComponent::FindMutableStack(EOceanResourceType ResourceType)
{
	return Stacks.FindByPredicate([ResourceType](const FOceanResourceStack& Stack) { return Stack.ResourceType == ResourceType; });
}

const FOceanResourceStack* UOceanInventoryComponent::FindStack(EOceanResourceType ResourceType) const
{
	return Stacks.FindByPredicate([ResourceType](const FOceanResourceStack& Stack) { return Stack.ResourceType == ResourceType; });
}
```

- [ ] **Step 5: Implement survival component**

Create `Source/Ocean/OceanPrototype/OceanSurvivalComponent.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanSurvivalComponent.generated.h"

UCLASS(ClassGroup=(Ocean), meta=(BlueprintSpawnableComponent))
class OCEAN_API UOceanSurvivalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanSurvivalComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Ocean|Survival")
	void ApplySurvivalDelta(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category="Ocean|Survival")
	void SetStats(float NewStamina, float NewHydration, float NewSatiety);

	UFUNCTION(BlueprintCallable, Category="Ocean|Survival")
	void SetHydrationDrainPerSecond(float NewValue) { HydrationDrainPerSecond = FMath::Max(0.0f, NewValue); }

	UFUNCTION(BlueprintCallable, Category="Ocean|Survival")
	void SetSatietyDrainPerSecond(float NewValue) { SatietyDrainPerSecond = FMath::Max(0.0f, NewValue); }

	UFUNCTION(BlueprintCallable, Category="Ocean|Survival")
	void SetStaminaRecoverPerSecond(float NewValue) { StaminaRecoverPerSecond = FMath::Max(0.0f, NewValue); }

	UFUNCTION(BlueprintPure, Category="Ocean|Survival")
	float GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintPure, Category="Ocean|Survival")
	float GetHydration() const { return Hydration; }

	UFUNCTION(BlueprintPure, Category="Ocean|Survival")
	float GetSatiety() const { return Satiety; }

private:
	UPROPERTY(EditAnywhere, Category="Ocean|Survival")
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, Category="Ocean|Survival")
	float Hydration = 100.0f;

	UPROPERTY(EditAnywhere, Category="Ocean|Survival")
	float Satiety = 100.0f;

	UPROPERTY(EditAnywhere, Category="Ocean|Survival")
	float HydrationDrainPerSecond = 0.4f;

	UPROPERTY(EditAnywhere, Category="Ocean|Survival")
	float SatietyDrainPerSecond = 0.25f;

	UPROPERTY(EditAnywhere, Category="Ocean|Survival")
	float StaminaRecoverPerSecond = 2.0f;
};
```

Create `Source/Ocean/OceanPrototype/OceanSurvivalComponent.cpp`:

```cpp
#include "OceanPrototype/OceanSurvivalComponent.h"

UOceanSurvivalComponent::UOceanSurvivalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UOceanSurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ApplySurvivalDelta(DeltaTime);
}

void UOceanSurvivalComponent::ApplySurvivalDelta(float DeltaSeconds)
{
	Stamina = FMath::Clamp(Stamina + StaminaRecoverPerSecond * DeltaSeconds, 0.0f, 100.0f);
	Hydration = FMath::Clamp(Hydration - HydrationDrainPerSecond * DeltaSeconds, 0.0f, 100.0f);
	Satiety = FMath::Clamp(Satiety - SatietyDrainPerSecond * DeltaSeconds, 0.0f, 100.0f);
}

void UOceanSurvivalComponent::SetStats(float NewStamina, float NewHydration, float NewSatiety)
{
	Stamina = FMath::Clamp(NewStamina, 0.0f, 100.0f);
	Hydration = FMath::Clamp(NewHydration, 0.0f, 100.0f);
	Satiety = FMath::Clamp(NewSatiety, 0.0f, 100.0f);
}
```

- [ ] **Step 6: Attach components to the character**

Modify `Source/Ocean/OceanCharacter.h`:

```cpp
class UOceanInventoryComponent;
class UOceanSurvivalComponent;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ocean", meta=(AllowPrivateAccess="true"))
TObjectPtr<UOceanInventoryComponent> InventoryComponent;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ocean", meta=(AllowPrivateAccess="true"))
TObjectPtr<UOceanSurvivalComponent> SurvivalComponent;

UFUNCTION(BlueprintPure, Category="Ocean")
UOceanInventoryComponent* GetInventoryComponent() const { return InventoryComponent.Get(); }

UFUNCTION(BlueprintPure, Category="Ocean")
UOceanSurvivalComponent* GetSurvivalComponent() const { return SurvivalComponent.Get(); }
```

Modify `Source/Ocean/OceanCharacter.cpp` constructor:

```cpp
InventoryComponent = CreateDefaultSubobject<UOceanInventoryComponent>(TEXT("OceanInventory"));
SurvivalComponent = CreateDefaultSubobject<UOceanSurvivalComponent>(TEXT("OceanSurvival"));
```

- [ ] **Step 7: Run build and tests**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --build-only
```

Expected: build exits with code `0`.

- [ ] **Step 8: Commit inventory and survival components**

Run:

```powershell
git add Source/Ocean/OceanPrototype/OceanResourceTypes.h Source/Ocean/OceanPrototype/OceanInventoryComponent.* Source/Ocean/OceanPrototype/OceanSurvivalComponent.* Source/Ocean/Tests/OceanMVPSurvivalTests.cpp Source/Ocean/OceanCharacter.*
git commit -m "feat: add Ocean survival and inventory components"
```

---

## Task 3: Interaction Interface And Resource Pickup

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanInteractableInterface.h`
- Create: `Source/Ocean/OceanPrototype/OceanInteractionComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanInteractionComponent.cpp`
- Create: `Source/Ocean/Tests/OceanMVPInteractionTests.cpp`
- Modify: `Source/Ocean/OceanPrototype/OceanResourceNode.h`
- Modify: `Source/Ocean/OceanPrototype/OceanResourceNode.cpp`
- Modify: `Source/Ocean/OceanCharacter.h`
- Modify: `Source/Ocean/OceanCharacter.cpp`
- Modify: `Source/Ocean/OceanPlayerController.cpp`

- [ ] **Step 1: Write failing interaction test**

Create `Source/Ocean/Tests/OceanMVPInteractionTests.cpp`:

```cpp
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanResourceNode.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPResourcePickupTest, "Ocean.MVP.Interaction.ResourcePickup", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPResourcePickupTest::RunTest(const FString& Parameters)
{
	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	AOceanResourceNode* Node = NewObject<AOceanResourceNode>();

	const FOceanResourceStack Stack = Node->Collect();
	TestTrue(TEXT("Collected stack has positive amount"), Stack.Amount > 0);
	TestTrue(TEXT("Inventory accepts collected stack"), Inventory->AddResource(Stack));
	TestEqual(TEXT("Inventory contains collected resource"), Inventory->GetResourceAmount(Stack.ResourceType), Stack.Amount);

	return true;
}

#endif
```

- [ ] **Step 2: Add the interaction interface**

Create `Source/Ocean/OceanPrototype/OceanInteractableInterface.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OceanInteractableInterface.generated.h"

UINTERFACE(BlueprintType)
class OCEAN_API UOceanInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class OCEAN_API IOceanInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Ocean|Interaction")
	FText GetOceanInteractionText(AActor* Interactor) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Ocean|Interaction")
	bool CanOceanInteract(AActor* Interactor, FText& FailureReason) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Ocean|Interaction")
	bool ExecuteOceanInteraction(AActor* Interactor, FText& OutMessage);
};
```

- [ ] **Step 3: Add the interaction component**

Create `Source/Ocean/OceanPrototype/OceanInteractionComponent.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanInteractionComponent.generated.h"

UCLASS(ClassGroup=(Ocean), meta=(BlueprintSpawnableComponent))
class OCEAN_API UOceanInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanInteractionComponent();

	UFUNCTION(BlueprintCallable, Category="Ocean|Interaction")
	AActor* FindBestInteractable() const;

	UFUNCTION(BlueprintCallable, Category="Ocean|Interaction")
	bool TryInteract(FText& OutMessage);

	UFUNCTION(BlueprintCallable, Category="Ocean|Interaction")
	void SetInteractionRadius(float NewRadius);

private:
	UPROPERTY(EditAnywhere, Category="Ocean|Interaction")
	float InteractionRadius = 250.0f;
};
```

Create `Source/Ocean/OceanPrototype/OceanInteractionComponent.cpp` with sphere overlap and nearest valid selection:

```cpp
#include "OceanPrototype/OceanInteractionComponent.h"
#include "OceanPrototype/OceanInteractableInterface.h"
#include "Engine/World.h"

UOceanInteractionComponent::UOceanInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AActor* UOceanInteractionComponent::FindBestInteractable() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return nullptr;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(OceanInteraction), false, Owner);
	World->OverlapMultiByObjectType(Overlaps, Owner->GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams(ECC_WorldDynamic), FCollisionShape::MakeSphere(InteractionRadius), Params);

	AActor* BestActor = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Candidate = Overlap.GetActor();
		if (!Candidate || !Candidate->GetClass()->ImplementsInterface(UOceanInteractableInterface::StaticClass()))
		{
			continue;
		}

		FText FailureReason;
		if (!IOceanInteractableInterface::Execute_CanOceanInteract(Candidate, const_cast<AActor*>(Owner), FailureReason))
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(Owner->GetActorLocation(), Candidate->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestActor = Candidate;
		}
	}

	return BestActor;
}

bool UOceanInteractionComponent::TryInteract(FText& OutMessage)
{
	AActor* BestActor = FindBestInteractable();
	if (!BestActor)
	{
		OutMessage = FText::FromString(TEXT("没有可交互目标"));
		return false;
	}

	return IOceanInteractableInterface::Execute_ExecuteOceanInteraction(BestActor, GetOwner(), OutMessage);
}

void UOceanInteractionComponent::SetInteractionRadius(float NewRadius)
{
	InteractionRadius = FMath::Max(1.0f, NewRadius);
}
```

- [ ] **Step 4: Implement resource node interaction**

Modify `AOceanResourceNode` to inherit `IOceanInteractableInterface` and add:

```cpp
virtual FText GetOceanInteractionText_Implementation(AActor* Interactor) const override;
virtual bool CanOceanInteract_Implementation(AActor* Interactor, FText& FailureReason) const override;
virtual bool ExecuteOceanInteraction_Implementation(AActor* Interactor, FText& OutMessage) override;
```

In `OceanResourceNode.cpp`, implement:

```cpp
FText AOceanResourceNode::GetOceanInteractionText_Implementation(AActor* Interactor) const
{
	return FText::Format(NSLOCTEXT("Ocean", "PickupResource", "F 拾取 {0} x{1}"), UEnum::GetDisplayValueAsText(ResourceType), Amount);
}

bool AOceanResourceNode::CanOceanInteract_Implementation(AActor* Interactor, FText& FailureReason) const
{
	if (Amount <= 0)
	{
		FailureReason = NSLOCTEXT("Ocean", "ResourceEmpty", "资源已被拾取");
		return false;
	}
	return true;
}

bool AOceanResourceNode::ExecuteOceanInteraction_Implementation(AActor* Interactor, FText& OutMessage)
{
	if (!Interactor)
	{
		OutMessage = NSLOCTEXT("Ocean", "NoInteractor", "没有交互者");
		return false;
	}

	UOceanInventoryComponent* Inventory = Interactor->FindComponentByClass<UOceanInventoryComponent>();
	if (!Inventory)
	{
		OutMessage = NSLOCTEXT("Ocean", "NoInventory", "没有背包");
		return false;
	}

	const FOceanResourceStack Stack = GetResourceStack();
	if (!Inventory->AddResource(Stack))
	{
		OutMessage = NSLOCTEXT("Ocean", "InventoryFull", "背包已满");
		return false;
	}

	Collect();
	OutMessage = NSLOCTEXT("Ocean", "PickupSuccess", "已拾取资源");
	Destroy();
	return true;
}
```

- [ ] **Step 5: Attach interaction component and wire F**

Add `UOceanInteractionComponent` to `AOceanCharacter`.

Modify `AOceanPlayerController::OnInteractTriggered`:

```cpp
void AOceanPlayerController::OnInteractTriggered(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	if (UOceanInteractionComponent* Interaction = ControlledPawn->FindComponentByClass<UOceanInteractionComponent>())
	{
		FText Message;
		Interaction->TryInteract(Message);
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanInteractResult: %s"), *Message.ToString());
	}
}
```

- [ ] **Step 6: Build and commit interaction**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --build-only
```

Expected: build exits with code `0`.

Commit:

```powershell
git add Source/Ocean/OceanPrototype/OceanInteractableInterface.h Source/Ocean/OceanPrototype/OceanInteractionComponent.* Source/Ocean/OceanPrototype/OceanResourceNode.* Source/Ocean/Tests/OceanMVPInteractionTests.cpp Source/Ocean/OceanCharacter.* Source/Ocean/OceanPlayerController.*
git commit -m "feat: add Ocean interaction and pickup flow"
```

---

## Task 4: Build Component And Deck Placement

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanBuildComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanBuildComponent.cpp`
- Create: `Source/Ocean/Tests/OceanMVPBuildTests.cpp`
- Modify: `Source/Ocean/OceanCharacter.h`
- Modify: `Source/Ocean/OceanCharacter.cpp`
- Modify: `Source/Ocean/OceanPlayerController.cpp`

- [ ] **Step 1: Write failing deck placement test**

Create `Source/Ocean/Tests/OceanMVPBuildTests.cpp`:

```cpp
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "OceanPrototype/OceanBuildGridComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanMVPDeckPlacementTest, "Ocean.MVP.Build.DeckPlacement", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOceanMVPDeckPlacementTest::RunTest(const FString& Parameters)
{
	UOceanBuildGridComponent* Grid = NewObject<UOceanBuildGridComponent>();
	Grid->ReserveFootprint({FIntPoint(0, 0)}, TEXT("Core"));

	UOceanInventoryComponent* Inventory = NewObject<UOceanInventoryComponent>();
	Inventory->AddResource({EOceanResourceType::Wood, 3});

	const TArray<FIntPoint> Footprint = UOceanBuildGridComponent::BuildFootprint(FIntPoint(1, 0), FIntPoint(1, 1), 0);
	TestTrue(TEXT("Adjacent deck can be placed"), Grid->CanPlaceFootprint(Footprint, true));
	TestTrue(TEXT("Can afford deck cost"), Inventory->CanAfford({{EOceanResourceType::Wood, 2}}));
	TestTrue(TEXT("Deck cost can be spent"), Inventory->TrySpend({{EOceanResourceType::Wood, 2}}));
	Grid->ReserveFootprint(Footprint, TEXT("Deck"));
	TestTrue(TEXT("Deck cell becomes occupied"), Grid->IsCellOccupied(FIntPoint(1, 0)));
	TestEqual(TEXT("Wood after deck"), Inventory->GetResourceAmount(EOceanResourceType::Wood), 1);

	return true;
}

#endif
```

- [ ] **Step 2: Implement build component**

Create `Source/Ocean/OceanPrototype/OceanBuildComponent.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanResourceTypes.h"
#include "OceanBuildComponent.generated.h"

class AOceanBuildModuleActor;
class AOceanFloatingPlatform;
class UOceanBuildModuleDefinition;

UCLASS(ClassGroup=(Ocean), meta=(BlueprintSpawnableComponent))
class OCEAN_API UOceanBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanBuildComponent();

	UFUNCTION(BlueprintCallable, Category="Ocean|Build")
	void ToggleBuildMode();

	UFUNCTION(BlueprintCallable, Category="Ocean|Build")
	void RotatePreview();

	UFUNCTION(BlueprintCallable, Category="Ocean|Build")
	bool TryPlaceSelectedModuleAtWorld(FVector WorldLocation, FText& OutMessage);

	UFUNCTION(BlueprintCallable, Category="Ocean|Build")
	void SetTargetPlatform(AOceanFloatingPlatform* Platform);

	UFUNCTION(BlueprintPure, Category="Ocean|Build")
	bool IsBuildModeActive() const { return bBuildModeActive; }

private:
	UPROPERTY(EditAnywhere, Category="Ocean|Build")
	TObjectPtr<UOceanBuildModuleDefinition> SelectedModule;

	UPROPERTY(EditAnywhere, Category="Ocean|Build")
	TObjectPtr<AOceanFloatingPlatform> TargetPlatform;

	UPROPERTY(EditAnywhere, Category="Ocean|Build")
	TSubclassOf<AOceanBuildModuleActor> FallbackModuleActorClass;

	bool bBuildModeActive = false;
	int32 RotationQuarterTurns = 0;
};
```

Implement `TryPlaceSelectedModuleAtWorld` by:

1. Getting owner inventory with `FindComponentByClass<UOceanInventoryComponent>()`.
2. Getting `TargetPlatform->GetBuildGrid()`.
3. Converting world location to anchor cell.
4. Reading selected module footprint and cost.
5. Checking grid placement and inventory affordability.
6. Spending resources.
7. Reserving footprint with a generated `FName`.
8. Spawning `AOceanBuildModuleActor` or selected module class at `CellToWorld(anchor)`.
9. Calling `ConfigurePlacedModule`.

- [ ] **Step 3: Wire build input in controller**

Modify `OnToggleBuildTriggered`:

```cpp
void AOceanPlayerController::OnToggleBuildTriggered(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (UOceanBuildComponent* Build = ControlledPawn->FindComponentByClass<UOceanBuildComponent>())
		{
			Build->ToggleBuildMode();
		}
	}
}
```

Modify `OnRotateBuildTriggered` similarly with `Build->RotatePreview()`.

Modify `OnSetDestinationReleased` so build mode intercepts short-click placement:

```cpp
if (APawn* ControlledPawn = GetPawn())
{
	if (UOceanBuildComponent* Build = ControlledPawn->FindComponentByClass<UOceanBuildComponent>())
	{
		if (Build->IsBuildModeActive())
		{
			FText Message;
			Build->TryPlaceSelectedModuleAtWorld(CachedDestination, Message);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildResult: %s"), *Message.ToString());
			FollowTime = 0.f;
			return;
		}
	}
}
```

- [ ] **Step 4: Attach build component to character**

Add `UOceanBuildComponent` to `AOceanCharacter` as `OceanBuild`.

- [ ] **Step 5: Build and commit build component**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --build-only
```

Expected: build exits with code `0`.

Commit:

```powershell
git add Source/Ocean/OceanPrototype/OceanBuildComponent.* Source/Ocean/Tests/OceanMVPBuildTests.cpp Source/Ocean/OceanCharacter.* Source/Ocean/OceanPlayerController.*
git commit -m "feat: add Ocean deck build component"
```

---

## Task 5: Debug HUD For Survival, Inventory, Prompt, And Build Mode

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanSurvivalHUD.h`
- Create: `Source/Ocean/OceanPrototype/OceanSurvivalHUD.cpp`
- Modify: `Source/Ocean/OceanGameMode.cpp`

- [ ] **Step 1: Implement HUD class**

Create `Source/Ocean/OceanPrototype/OceanSurvivalHUD.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OceanSurvivalHUD.generated.h"

UCLASS()
class OCEAN_API AOceanSurvivalHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
```

Create `Source/Ocean/OceanPrototype/OceanSurvivalHUD.cpp`:

```cpp
#include "OceanPrototype/OceanSurvivalHUD.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanInteractionComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanInteractableInterface.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "GameFramework/Pawn.h"

void AOceanSurvivalHUD::DrawHUD()
{
	Super::DrawHUD();

	APawn* Pawn = GetOwningPawn();
	if (!Pawn || !Canvas)
	{
		return;
	}

	float Y = 40.0f;
	const float X = 40.0f;

	if (const UOceanSurvivalComponent* Survival = Pawn->FindComponentByClass<UOceanSurvivalComponent>())
	{
		DrawText(FString::Printf(TEXT("体力 %.0f | 水分 %.0f | 饱食 %.0f"), Survival->GetStamina(), Survival->GetHydration(), Survival->GetSatiety()), FColor::White, X, Y);
		Y += 24.0f;
	}

	if (const UOceanInventoryComponent* Inventory = Pawn->FindComponentByClass<UOceanInventoryComponent>())
	{
		DrawText(FString::Printf(TEXT("木材 %d | 碎片 %d | 食物 %d | 水 %d"),
			Inventory->GetResourceAmount(EOceanResourceType::Wood),
			Inventory->GetResourceAmount(EOceanResourceType::Scrap),
			Inventory->GetResourceAmount(EOceanResourceType::Food),
			Inventory->GetResourceAmount(EOceanResourceType::Water)), FColor::Cyan, X, Y);
		Y += 24.0f;
	}

	if (const UOceanBuildComponent* Build = Pawn->FindComponentByClass<UOceanBuildComponent>())
	{
		DrawText(Build->IsBuildModeActive() ? TEXT("建造模式：左键放置，R 旋转，B 退出") : TEXT("B 打开建造模式"), FColor::Yellow, X, Y);
		Y += 24.0f;
	}

	if (const UOceanInteractionComponent* Interaction = Pawn->FindComponentByClass<UOceanInteractionComponent>())
	{
		if (AActor* Target = Interaction->FindBestInteractable())
		{
			const FText Prompt = IOceanInteractableInterface::Execute_GetOceanInteractionText(Target, Pawn);
			DrawText(Prompt.ToString(), FColor::Green, X, Y);
		}
	}
}
```

- [ ] **Step 2: Set HUD class in game mode**

Modify `Source/Ocean/OceanGameMode.cpp`:

```cpp
#include "OceanPrototype/OceanSurvivalHUD.h"

AOceanGameMode::AOceanGameMode()
{
	HUDClass = AOceanSurvivalHUD::StaticClass();
}
```

- [ ] **Step 3: Build and commit HUD**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --build-only
```

Expected: build exits with code `0`.

Commit:

```powershell
git add Source/Ocean/OceanPrototype/OceanSurvivalHUD.* Source/Ocean/OceanGameMode.cpp
git commit -m "feat: add Ocean MVP debug HUD"
```

---

## Task 6: Editor Setup Script For Assets And Map Actors

**Files:**
- Create: `scripts/setup_mvp_survival_loop.py`
- Create: `scripts/verify_mvp_survival_loop.py`
- Modify: `docs/production/2026-06-16-mvp-survival-loop/05-implementation-log.md`

- [ ] **Step 1: Create setup script**

Create `scripts/setup_mvp_survival_loop.py` with these responsibilities:

```python
import unreal

MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"

def load_map():
    unreal.EditorLevelLibrary.load_level(MAP_PATH)

def ensure_folder(path):
    unreal.EditorAssetLibrary.make_directory(path)

def ensure_input_assets():
    ensure_folder("/Game/OceanPrototype/Input")
    # Create InputAction assets with AssetTools when missing:
    # IA_OceanMove: Axis2D, WASD mapping in IMC_OceanMVP
    # IA_OceanInteract: Digital, F
    # IA_OceanToggleBuild: Digital, B
    # IA_OceanRotateBuild: Digital, R
    # Keep existing TopDown click action mappings in the inherited mapping context.

def ensure_blueprints_and_data():
    ensure_folder("/Game/OceanPrototype/Blueprints")
    ensure_folder("/Game/OceanPrototype/Build")
    # Create BP_OceanSurvivorCharacter derived from /Script/Ocean.OceanCharacter.
    # Create BP_OceanMVPGameMode derived from /Script/Ocean.OceanGameMode.
    # Create DA_BuildModule_Deck_1x1 derived from /Script/Ocean.OceanBuildModuleDefinition.

def ensure_map_actors():
    # Spawn or find:
    # - OceanFloatingPlatform_Starter
    # - OceanResourceField_Starter
    # - PlayerStart_MVP
    # Configure resource field fallback count and exclusion radius.
    # Save all dirty packages.

def main():
    load_map()
    ensure_input_assets()
    ensure_blueprints_and_data()
    ensure_map_actors()
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    print("[TDD] MVPSetupSave: PASS")

if __name__ == "__main__":
    main()
```

When implementing, use the existing `scripts/create_water_ocean_map.py` style for asset creation and `[TDD]` logging. Keep this script idempotent: rerunning it should update existing assets/actors, not duplicate them.

- [ ] **Step 2: Create verification script**

Create `scripts/verify_mvp_survival_loop.py`:

```python
import unreal

MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"

def count_actors_by_class(class_name):
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    return [actor for actor in actors if actor.get_class().get_name() == class_name]

def main():
    unreal.EditorLevelLibrary.load_level(MAP_PATH)
    print("[TDD] MVPMapLoaded: PASS")

    required_assets = [
        "/Game/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter",
        "/Game/OceanPrototype/Blueprints/BP_OceanMVPGameMode",
        "/Game/OceanPrototype/Input/IMC_OceanMVP",
        "/Game/OceanPrototype/Build/DA_BuildModule_Deck_1x1",
    ]
    for asset_path in required_assets:
        exists = unreal.EditorAssetLibrary.does_asset_exist(asset_path)
        print(f"[TDD] MVPAssetExists path={asset_path} result={'PASS' if exists else 'FAIL'}")

    platform_count = len(count_actors_by_class("OceanFloatingPlatform"))
    resource_field_count = len(count_actors_by_class("OceanResourceField"))
    print(f"[TDD] MVPPlatformCount actual={platform_count} expected>=1")
    print(f"[TDD] MVPResourceFieldCount actual={resource_field_count} expected>=1")

if __name__ == "__main__":
    main()
```

- [ ] **Step 3: Compile scripts**

Run:

```powershell
python -m py_compile scripts/setup_mvp_survival_loop.py scripts/verify_mvp_survival_loop.py
```

Expected: command exits with code `0`.

- [ ] **Step 4: Run setup through UnrealBridge**

Run:

```powershell
python scripts/ue_tdd_bridge.py --exec-file scripts/setup_mvp_survival_loop.py
python scripts/ue_tdd_bridge.py --exec-file scripts/verify_mvp_survival_loop.py
```

Expected: verification emits `PASS` for required assets and `actual>=1` for platform/resource field.

- [ ] **Step 5: Commit setup scripts and generated assets**

Run:

```powershell
git add scripts/setup_mvp_survival_loop.py scripts/verify_mvp_survival_loop.py Content/OceanPrototype docs/production/2026-06-16-mvp-survival-loop/05-implementation-log.md
git commit -m "content: set up Ocean MVP map assets"
```

---

## Task 7: Full No-LiveCoding Verification And Documentation Sync

**Files:**
- Modify: `memory-bank/architecture.md`
- Modify: `memory-bank/progress.md`
- Modify: `docs/defense/commit-log.md`
- Modify: `docs/production/2026-06-16-mvp-survival-loop/06-test-results.md`
- Modify: `docs/production/2026-06-16-mvp-survival-loop/07-review.md`

- [ ] **Step 1: Run production validator**

Run:

```powershell
python scripts/harness_state_validator.py --json
```

Expected: JSON output has `"errors": []`.

- [ ] **Step 2: Run full pipeline**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --pie-duration 5
```

Expected:

- Editor dirty packages are saved before shutdown.
- `OceanEditor Win64 Development -NoHotReload` builds successfully.
- Editor relaunches.
- PIE runs for five seconds.
- Logs include MVP `[TDD]` checks from setup/verification scripts when those scripts are executed.

- [ ] **Step 3: Run verification script in editor**

Run:

```powershell
python scripts/ue_tdd_bridge.py --exec-file scripts/verify_mvp_survival_loop.py
```

Expected:

```text
[TDD] MVPMapLoaded: PASS
[TDD] MVPAssetExists path=/Game/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter result=PASS
[TDD] MVPAssetExists path=/Game/OceanPrototype/Blueprints/BP_OceanMVPGameMode result=PASS
[TDD] MVPAssetExists path=/Game/OceanPrototype/Input/IMC_OceanMVP result=PASS
[TDD] MVPAssetExists path=/Game/OceanPrototype/Build/DA_BuildModule_Deck_1x1 result=PASS
[TDD] MVPPlatformCount actual=1 expected>=1
[TDD] MVPResourceFieldCount actual=1 expected>=1
```

- [ ] **Step 4: Update docs**

Update `memory-bank/architecture.md` with:

- `UOceanInventoryComponent`
- `UOceanSurvivalComponent`
- `UOceanInteractionComponent`
- `UOceanBuildComponent`
- `AOceanSurvivalHUD`

Update `memory-bank/progress.md` with:

- latest build command and result;
- setup script result;
- verification script result;
- remaining gaps for fishing/diving/island events.

Append `docs/defense/commit-log.md` with each commit created by this plan and its defense value.

Update `docs/production/2026-06-16-mvp-survival-loop/06-test-results.md` with actual command output summaries.

Update `docs/production/2026-06-16-mvp-survival-loop/07-review.md` with defects, missing tests, and next steps.

- [ ] **Step 5: Run final static checks**

Run:

```powershell
python scripts/doc_sync_hook.py --phase pre-commit --apply-memory --history --quiet
git diff --check
git status --short
```

Expected:

- doc sync exits with code `0`;
- `git diff --check` prints no errors;
- status shows only files intended for the final docs/checkpoint commit.

- [ ] **Step 6: Commit documentation sync**

Run:

```powershell
git add memory-bank/architecture.md memory-bank/progress.md docs/defense/commit-log.md docs/production/2026-06-16-mvp-survival-loop
git commit -m "docs: record Ocean MVP survival loop verification"
```

- [ ] **Step 7: Push branch**

Run:

```powershell
git push origin codex/ocean-phase-one-workflows
```

Expected: branch pushes successfully to `HOMO0523/OceanGame.git`.

---

## Self-Review

### Spec Coverage

- Movement: Task 1 keeps left-click and adds WASD.
- Interaction: Task 3 adds `F` interaction and resource pickup.
- Survival stats: Task 2 adds stamina, hydration, satiety.
- Inventory: Task 2 adds stack inventory with capacity and spending.
- Build growth: Task 4 adds one deck placement path through existing grid rules.
- HUD: Task 5 displays status, inventory, prompt, and build mode.
- Map/assets: Task 6 creates Ocean MVP input assets, blueprint classes, deck data asset, starter platform, and resource field.
- Verification/documentation: Task 7 runs pipeline checks and records evidence.
- Deferred fishing/diving/island/cruise features: explicitly out of scope and not implemented by any task.

### Placeholder Scan

The plan names exact files, commands, expected outputs, class names, method names, and component responsibilities. The only generated-asset sections are assigned to `scripts/setup_mvp_survival_loop.py`, which is intentionally the task that creates those assets.

### Type Consistency

- Resource type enum values used by tests match `EOceanResourceType`.
- Movement helper is `FOceanInputMath::MakeCameraRelativeMoveDirection` in both test and implementation.
- Component names used by HUD, controller, and character match the C++ class names.
- Build placement consumes `UOceanInventoryComponent`, `UOceanBuildGridComponent`, `UOceanBuildModuleDefinition`, and `AOceanBuildModuleActor`, matching existing OceanPrototype foundation types.
