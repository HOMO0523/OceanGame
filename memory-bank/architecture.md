# Architecture — Ocean

## 1. Project Shape

Ocean is a UE 5.7 single-player prototype focused on a floating ocean survival/building loop.

Current runtime module:

```text
Ocean
└── OceanPrototype
    ├── Input, survival, inventory, and interaction MVP loop
    ├── Build grid and module placement
    ├── Floating platform core
    ├── Floating resource nodes
    └── PCG/fallback resource field
```

## 2. Core Systems

| System | File/Area | Responsibility |
|---|---|---|
| Player input | `Source/Ocean/OceanPlayerController.*`, `/Game/OceanPrototype/Input/*` | Keeps click-to-move while adding WASD movement, F interaction, B build toggle, and R build rotation through `BP_OceanMVPPlayerController`. |
| Survival / inventory | `UOceanSurvivalComponent`, `UOceanInventoryComponent` | Tracks health-adjacent MVP pressure through stamina, hydration, satiety, and stackable resource costs. |
| Interaction | `UOceanInteractionComponent`, `UOceanInteractableInterface` | Finds the nearest valid interactable and routes F interaction to resources or later event actors. |
| Build grid | `Source/Ocean/OceanPrototype/OceanBuildGridComponent.*` | Stable logical cells, footprint rotation, overlap and adjacency checks. |
| Build placement | `Source/Ocean/OceanPrototype/OceanBuildComponent.*` | Toggles build mode, rotates previews, consumes inventory cost, and auto-resolves the floating platform if no explicit target is assigned. |
| Module definition | `Source/Ocean/OceanPrototype/OceanBuildModuleDefinition.*` | Data-asset contract for footprint, cost, preview mesh, and actor class. |
| Module actor | `Source/Ocean/OceanPrototype/OceanBuildModuleActor.*` | Cube-placeholder build module with inherited buoyancy path. |
| Floating platform | `Source/Ocean/OceanPrototype/OceanFloatingPlatform.*` | Owns stable grid and visual bobbing root. |
| Resource node | `Source/Ocean/OceanPrototype/OceanResourceNode.*` | Collectable floating resource actor with cube placeholder and buoyancy. |
| Resource field | `Source/Ocean/OceanPrototype/OceanResourceField.*` | Owns PCG component and deterministic fallback spawn positions. |
| Debug HUD | `Source/Ocean/OceanPrototype/OceanSurvivalHUD.*` | Draws survival stats, inventory counts, build state, and nearest interaction prompt for MVP debugging. |
| MVP map content | `scripts/setup_mvp_survival_loop.py`, `scripts/verify_mvp_survival_loop.py` | Generates and verifies `L_WaterOcean` starter platform, resource field, input assets, Blueprint classes, and the 1x1 deck data asset. |

## 3. Data Flow

### Build Placement

1. `BP_OceanSurvivorCharacter` defaults its build component to `DA_BuildModule_Deck_1x1`.
2. Placement converts world position to grid anchor with `UOceanBuildGridComponent::WorldToCell`.
3. Footprint cells are generated from module size and rotation.
4. `CanPlaceFootprint` rejects overlaps and optionally enforces adjacency.
5. `ReserveFootprint` records occupied cells by module id.
6. Spawned module actor keeps buoyancy and placeholder mesh until user replaces the visual asset.
7. If `TargetPlatform` was not explicitly assigned, `UOceanBuildComponent` resolves the first valid `AOceanFloatingPlatform` in the world and caches it.

### Player Loop

1. `BP_OceanMVPGameMode` spawns `BP_OceanSurvivorCharacter` with `BP_OceanMVPPlayerController`.
2. `IMC_OceanMVP` binds WASD, F, B, R, left mouse, and touch into one Enhanced Input context.
3. Floating `AOceanResourceNode` actors expose F pickup and feed `UOceanInventoryComponent`.
4. Build mode consumes inventory resources to place `AOceanBuildModuleActor` cells adjacent to the starter platform.

### Resource Spawn

1. PCG is the preferred authoring path for scatter rules.
2. `AOceanResourceField` provides deterministic fallback locations for tests and early maps.
3. Spawned `AOceanResourceNode` actors float on the water and return `FOceanResourceStack` when collected.

## 4. Plugin Boundaries

| Plugin | Role | Boundary |
|---|---|---|
| Water | Ocean surface, water collision profile, buoyancy sampling. | Required for water actors and `UBuoyancyComponent`. |
| PCG | Resource scattering and future island/loot generation. | Runtime actors can provide deterministic fallback until PCG graph assets exist. |
| UnrealBridge | Editor automation, Python execution, PIE/log capture, save gate. | Editor-only; never required by packaged runtime. |

## 5. Prototype Rules

- Module is a group of grid cells, not necessarily one cell.
- Logical grid is stable even when visuals bob on waves.
- Missing models use cube placeholders first.
- Floating actor assets must retain buoyancy components.
- Water setup is incomplete unless `WaterBodyCollision` exists in `Config/DefaultEngine.ini`.
- Fishing, diving, island travel, and cruise intro remain out of MVP scope until the small-boat loop is playable and testable.
