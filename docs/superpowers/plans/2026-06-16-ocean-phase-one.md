# Ocean Phase One Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the first playable Ocean prototype foundation: Water/PCG enabled, floating cube-placeholder actors with buoyancy components, grid/module construction core, and reusable project workflows.

**Architecture:** Keep gameplay logic in focused C++ classes under `Source/Ocean/OceanPrototype`, with placeholder meshes loaded from Engine basic shapes until final assets are supplied. Grid validation stays independent from visual bobbing and buoyancy so build placement remains deterministic. Editor-created maps, PCG graphs, and Blueprint assets can be generated later on top of these stable actor classes.

**Tech Stack:** Unreal Engine 5.7, C++, Water plugin (`UBuoyancyComponent`), PCG plugin (`UPCGComponent`), Enhanced Input later, UE Automation Tests, PowerShell cold builds.

---

## File Structure

- Modify: `Ocean.uproject` to enable `Water` and `PCG`.
- Modify: `Source/Ocean/Ocean.Build.cs` to add Water/PCG and automation-test dependencies.
- Create: `Source/Ocean/OceanPrototype/OceanResourceTypes.h` for resource enums and cost structs.
- Create: `Source/Ocean/OceanPrototype/OceanBuildGridComponent.h/.cpp` for grid coordinate, footprint, and occupancy rules.
- Create: `Source/Ocean/OceanPrototype/OceanBuildModuleDefinition.h/.cpp` for module data assets.
- Create: `Source/Ocean/OceanPrototype/OceanBuildModuleActor.h/.cpp` for placeable cube-placeholder modules with buoyancy.
- Create: `Source/Ocean/OceanPrototype/OceanFloatingPlatform.h/.cpp` for the stable logical platform and initial occupied cells.
- Create: `Source/Ocean/OceanPrototype/OceanResourceNode.h/.cpp` for collectable floating resources with buoyancy.
- Create: `Source/Ocean/OceanPrototype/OceanResourceField.h/.cpp` for a PCG-host actor and deterministic fallback resource spawning.
- Create: `Source/Ocean/Tests/OceanBuildGridTests.cpp` for automation coverage of grid placement behavior.
- Create: `docs/workflows/ocean-agent-workflows.md` for local workflow, testing, and skill guidance.

## Task 1: Workflow Documentation

**Files:**
- Create: `docs/workflows/ocean-agent-workflows.md`

- [ ] **Step 1: Write workflow guide**

Include operating rules, no-LiveCoding compile path, TDD loop, production-unit structure, skills list, and scripts to port from the reference project.

- [ ] **Step 2: Verify guide has no machine-specific project path**

Run: `Select-String -Path docs\workflows\ocean-agent-workflows.md -Pattern 'D:\\BallGame|D:\\UE5 demo'`

Expected: no matches.

- [ ] **Step 3: Commit**

Run:

```powershell
git add docs/workflows/ocean-agent-workflows.md
git commit -m "docs: add Ocean agent workflows"
```

## Task 2: Plugin And Build Setup

**Files:**
- Modify: `Ocean.uproject`
- Modify: `Source/Ocean/Ocean.Build.cs`

- [ ] **Step 1: Enable plugins**

Add `Water` and `PCG` entries to `Ocean.uproject`:

```json
{
  "Name": "Water",
  "Enabled": true
},
{
  "Name": "PCG",
  "Enabled": true
}
```

- [ ] **Step 2: Add module dependencies**

Add `Water` and `PCG` to `PublicDependencyModuleNames`. Add `DeveloperSettings` only if an implementation file needs settings classes.

- [ ] **Step 3: Build cold**

Run:

```powershell
& "D:\UE_5.7\Engine\Build\BatchFiles\Build.bat" OceanEditor Win64 Development "D:\UE5 demo\Ocean\Ocean.uproject" -NoHotReload
```

Expected: exit code `0`.

- [ ] **Step 4: Commit**

Run:

```powershell
git add Ocean.uproject Source/Ocean/Ocean.Build.cs
git commit -m "chore: enable Water and PCG"
```

## Task 3: Grid TDD

**Files:**
- Create: `Source/Ocean/Tests/OceanBuildGridTests.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanResourceTypes.h`
- Create: `Source/Ocean/OceanPrototype/OceanBuildGridComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanBuildGridComponent.cpp`

- [ ] **Step 1: Write failing automation tests**

Create tests for:

- `WorldToCell` snaps `FVector(149, 151, 0)` to `FIntPoint(1, 2)` with `CellSize=100`.
- A `2x1` footprint rotated by 90 degrees occupies two vertical cells.
- Placement fails when any footprint cell overlaps an occupied cell.
- Placement succeeds when a new footprint touches an existing deck edge.

- [ ] **Step 2: Run test build and verify red**

Run the cold build command from Task 2.

Expected: compile/test failure because the grid component API is not implemented yet.

- [ ] **Step 3: Implement grid component**

Implement:

- `WorldToCell`
- `CellToWorld`
- `BuildFootprint`
- `CanPlaceFootprint`
- `ReserveFootprint`
- `IsCellOccupied`
- `ClearGrid`

- [ ] **Step 4: Run automation-capable build**

Run the cold build command from Task 2.

Expected: exit code `0`.

- [ ] **Step 5: Commit**

Run:

```powershell
git add Source/Ocean/OceanPrototype Source/Ocean/Tests/OceanBuildGridTests.cpp
git commit -m "feat: add Ocean build grid"
```

## Task 4: Placeholder Module And Resource Actors

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanBuildModuleDefinition.h/.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanBuildModuleActor.h/.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanResourceNode.h/.cpp`

- [ ] **Step 1: Add component-presence tests**

Add automation tests or constructor checks proving module and resource actors create:

- `UStaticMeshComponent`
- `UBuoyancyComponent`
- cube placeholder mesh fallback when no mesh is provided

- [ ] **Step 2: Run red**

Run the cold build command from Task 2.

Expected: failure because actor classes are missing.

- [ ] **Step 3: Implement actors**

Implement cube-placeholder actors that:

- load `/Engine/BasicShapes/Cube.Cube` as fallback mesh;
- expose `SetPlaceholderMesh`;
- include `UBuoyancyComponent`;
- keep collision enabled for platform modules;
- set resource nodes to simulate physics and float.

- [ ] **Step 4: Build**

Run the cold build command from Task 2.

Expected: exit code `0`.

- [ ] **Step 5: Commit**

Run:

```powershell
git add Source/Ocean/OceanPrototype Source/Ocean/Tests/OceanBuildGridTests.cpp
git commit -m "feat: add buoyant placeholder actors"
```

## Task 5: Platform And Resource Field

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanFloatingPlatform.h/.cpp`
- Create: `Source/Ocean/OceanPrototype/OceanResourceField.h/.cpp`

- [ ] **Step 1: Add tests**

Add tests proving:

- platform initialization reserves the configured core grid footprint;
- fallback resource spawn positions are outside the exclusion radius;
- resource field owns a `UPCGComponent`.

- [ ] **Step 2: Run red**

Run the cold build command from Task 2.

Expected: failure because platform/resource-field classes are missing.

- [ ] **Step 3: Implement platform**

Implement `AOceanFloatingPlatform` with:

- `UOceanBuildGridComponent`;
- `InitialCoreSize`;
- `InitializeCorePlatform`;
- optional visual root bobbing separate from grid root.

- [ ] **Step 4: Implement resource field**

Implement `AOceanResourceField` with:

- `UPCGComponent`;
- `SpawnFallbackResources`;
- deterministic seed;
- exclusion radius around platform origin.

- [ ] **Step 5: Build**

Run the cold build command from Task 2.

Expected: exit code `0`.

- [ ] **Step 6: Commit**

Run:

```powershell
git add Source/Ocean/OceanPrototype Source/Ocean/Tests/OceanBuildGridTests.cpp
git commit -m "feat: add platform and resource field"
```

## Task 6: Verification And Push

**Files:**
- All changed files.

- [ ] **Step 1: Run final cold build**

Run:

```powershell
& "D:\UE_5.7\Engine\Build\BatchFiles\Build.bat" OceanEditor Win64 Development "D:\UE5 demo\Ocean\Ocean.uproject" -NoHotReload
```

Expected: exit code `0`.

- [ ] **Step 2: Run whitespace check**

Run: `git diff --check`

Expected: no output.

- [ ] **Step 3: Push feature branch**

Run:

```powershell
git push -u origin codex/ocean-phase-one-workflows
```

Expected: branch pushed.
