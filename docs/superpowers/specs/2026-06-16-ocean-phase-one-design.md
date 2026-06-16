# Ocean Phase One Design

## Context

`Ocean` is currently a UE 5.7 single-player prototype project based on Epic template content. The active project module is `Ocean`, the default map is still `/Game/TopDown/Lvl_TopDown`, and the enabled plugins are template-level editor/gameplay plugins plus StateTree. There is no existing Ocean-specific gameplay layer, Water setup, PCG graph, floating-platform logic, or build-system code.

Phase one establishes a playable vertical slice for a sea-survival base prototype: a visible ocean, an initial floating platform, grid-based construction where modules are groups of cells, and procedurally scattered resource nodes around the platform.

## Goals

- Create an isolated prototype area under `Content/OceanPrototype` so existing template maps and assets remain usable.
- Enable UE 5.7 Water and PCG plugin support for ocean visuals and resource scattering.
- Build a grid construction model where individual deck cells and larger modules share the same placement rules.
- Let the player preview, rotate, validate, and place modules on or adjacent to a floating platform.
- Scatter collectable resources in the surrounding ocean using PCG, with clear exclusion zones around the starting platform.
- Keep physics simple: platform logic uses a stable grid plane while visuals can show light floating motion.

## Non-Goals

- No multiplayer, save/load, storms, boats, enemies, or long-term economy in phase one.
- No fully simulated buoyancy for the platform grid. The grid remains stable for construction reliability.
- No full UI inventory panel. A debug HUD or lightweight resource counter is enough.
- No final art pass. Use LevelPrototyping meshes and simple materials until the core loop works.

## Plugin And Project Setup

The project should enable these plugins in `Ocean.uproject`:

- `Water`: provides `WaterZone` and `WaterBodyOcean` for ocean rendering. In this UE 5.7 install it is an experimental plugin at `D:\UE_5.7\Engine\Plugins\Experimental\Water\Water.uplugin`.
- `PCG`: provides PCG graphs/components for procedural resource placement. In this UE 5.7 install it is at `D:\UE_5.7\Engine\Plugins\PCG\PCG.uplugin`.

If C++ code directly includes Water or PCG runtime types, add `Water` and `PCG` to `PublicDependencyModuleNames` in `Source/Ocean/Ocean.Build.cs`. If phase-one PCG work remains entirely in editor-authored PCG Graph assets, the C++ dependency can wait until direct type usage is needed.

The default map should eventually point to `/Game/OceanPrototype/Maps/L_OceanPrototype`, but the first implementation pass can keep the existing template default until the map has been created and verified.

## Content Layout

Create new project-owned assets under:

- `Content/OceanPrototype/Maps`: `L_OceanPrototype`, the first playable ocean map.
- `Content/OceanPrototype/Build`: module definitions, preview materials, placed-module blueprints, and platform assets.
- `Content/OceanPrototype/Resources`: resource node actors, meshes, pickup effects, and PCG graph assets.
- `Content/OceanPrototype/Input`: build-mode input actions if the existing TopDown input set is not enough.
- `Content/OceanPrototype/UI`: a minimal build-mode/resource-counter widget if debug text becomes insufficient.

Template folders such as `Content/TopDown`, `Content/Variant_Strategy`, and `Content/LevelPrototyping` may be referenced, but Ocean-specific assets should live in the OceanPrototype folder.

## Player Experience

The player starts on a small floating platform in open water. Nearby drift resources are visible around the base. The player can collect resources, enter build mode, select a deck or module, move a ghost preview over the platform grid, rotate it in 90-degree increments, and place it when the preview is valid.

Construction uses an A+B model:

- A single deck tile is a `1x1` module.
- Larger structures are modules made from multiple occupied grid cells, such as `2x2` storage or `3x2` workshop modules.
- All modules use the same occupancy, rotation, adjacency, and cost checks.

The first playable loop is: collect resource → place deck/module → expanded platform changes valid build area → resources remain scattered outside the safe area.

## Architecture

### `AOceanPrototypeGameMode`

Owns phase-one rules and references the prototype player controller/pawn classes. It should be introduced only after the map can run without relying on the template TopDown blueprint game mode.

### `AOceanBuildPlayerController`

Handles mouse input, build-mode state, cursor-to-world projection, module rotation, preview confirmation, and cancellation. It should remain thin: input decisions live here, but grid validation lives on the platform/grid component.

### `AOceanFloatingPlatform`

Represents the logical base. It owns the build grid, initial core cells, placed module instances, and optional visual bobbing. Its logical plane should stay stable at a known Z height, even if visual child components animate slightly.

### `UOceanBuildGridComponent`

Maintains the construction grid:

- `CellSize`: world-space size of one build cell.
- `GridOrigin`: world-space origin for grid coordinate conversion.
- `OccupiedCells`: map/set from integer cell coordinates to placed module data.
- Coordinate helpers: world to cell, cell to world, rotated footprint expansion.
- Validation helpers: bounds, overlap, adjacency, foundation support, and placement reservation.

This component is the core gameplay boundary for construction. Other systems ask it whether a module can be placed; they do not duplicate grid math.

### `UOceanBuildModuleDefinition`

A data asset describing one buildable module:

- Display name.
- Footprint cells before rotation.
- Resource cost.
- Preview mesh/material.
- Placed actor or mesh class.
- Placement rules such as requires adjacency, can start on water, or requires existing deck support.

The same definition system supports `1x1` deck pieces and larger modules.

### `AOceanBuildModuleActor`

The spawned, placed representation of a module. It stores its module definition, occupied cells, rotation, and any later interaction hooks. Phase one only requires static placement.

### `AOceanResourceNode`

A collectable resource actor with resource type, amount, and an interaction radius. It should support at least two resource types in phase one, for example wood and scrap, so costs can prove that the system is data-driven.

### `AOceanResourceField`

An actor that owns or hosts the PCG component/graph for resource scattering. It defines generation bounds and exclusion settings around the starting platform.

## Build Placement Flow

1. Player enters build mode and selects a `UOceanBuildModuleDefinition`.
2. Controller traces from cursor to the platform/grid plane or existing platform collision.
3. The hit world location converts to a grid cell through `UOceanBuildGridComponent`.
4. The selected module footprint is rotated around its anchor cell.
5. The grid component validates:
   - every footprint cell is unoccupied;
   - at least one required adjacency/support rule is satisfied;
   - the player has enough resources;
   - placement is not inside the reserved start/core exclusion area unless the module is allowed there.
6. Preview actor updates location, rotation, and material color.
7. Left click confirms valid placement, spends resources, spawns `AOceanBuildModuleActor`, and reserves occupied cells.
8. Right click or ESC cancels the preview without changing inventory or grid state.

## Water Flow

1. Enable the Water plugin and restart the editor.
2. Create `L_OceanPrototype`.
3. Add a `WaterZone` actor large enough for the prototype arena.
4. Add a `WaterBodyOcean` actor at sea level.
5. Set the logical construction plane to a fixed value above water, for example `Z=120`.
6. Keep player movement/navmesh on platform collision, not directly on the water surface.
7. Add simple visual bobbing only to platform visual roots or resource nodes, not to the logical grid transform.

This avoids early instability where animated water, moving platform collision, and grid placement all fight each other.

## PCG Resource Scattering Flow

1. Create a PCG graph for ocean drift resources.
2. Generate points in a ring or rectangular field around the initial platform.
3. Reject points inside the platform safe radius and inside future reserved build-core space.
4. Randomly select resource node classes based on weighted type data.
5. Spawn `AOceanResourceNode` actors or static mesh instances plus lightweight interaction actors.
6. Keep the first version editor-generated or BeginPlay-generated; runtime regeneration can wait until the resource loop is proven.
7. Add a debug regeneration command or editor-exposed seed value so layout changes can be repeated.

The first PCG pass should prioritize readable gameplay spacing over visual density.

## Initial Module Set

Phase one should include only enough modules to prove the unified grid/module model:

- `Deck_1x1`: one-cell platform expansion, low wood cost, can attach to any existing deck edge.
- `Storage_2x2`: larger occupied area, higher wood/scrap cost, requires support from existing deck cells.
- `Workshop_3x2`: rectangular module that proves rotation and multi-cell footprint checks.

All three are modules. The deck tile is not a special system.

## Resource Model

Use a minimal resource inventory:

- `Wood`: common drift resource, used for deck expansion.
- `Scrap`: less common resource, used for larger modules.

The player controller or a small player-state component can hold the counts in phase one. If inventory grows later, it can move into a dedicated component without changing grid placement rules.

## Testing And Validation

Phase one is valid when these checks pass in PIE:

- The prototype map opens with a visible ocean and starting platform.
- The player can move on the platform without falling through or relying on water collision.
- Resource nodes spawn outside the starting platform exclusion zone.
- The player can collect resources and see counts change.
- `Deck_1x1` placement expands the platform by exactly one grid cell.
- `Storage_2x2` and `Workshop_3x2` occupy all expected cells after rotation.
- Invalid overlap previews are visibly rejected.
- Placement fails when resource cost is not met.
- Placed modules persist for the current PIE session and block subsequent placements.

## Recommended Implementation Order

1. Enable Water and PCG plugins, then restart/regenerate project files.
2. Create `Content/OceanPrototype` folders and `L_OceanPrototype`.
3. Add WaterZone, WaterBodyOcean, and a static starting platform.
4. Add C++ grid coordinate and footprint validation.
5. Add module data assets and three initial module definitions.
6. Add preview placement interaction and resource-cost checks.
7. Add resource node actor and collection logic.
8. Add PCG graph for resource scattering with exclusion zone.
9. Set prototype GameMode and map defaults after the vertical slice works.

## Open Decision

The project is not currently a git repository. The design is ready to implement either after initializing git for checkpoint commits or as an unversioned prototype pass. Initializing git is recommended before code changes so each phase can be rolled back cleanly.
