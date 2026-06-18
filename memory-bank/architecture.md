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
| Player input | `Source/Ocean/OceanPlayerController.*`, `/Game/OceanPrototype/Input/*` | Keeps click-to-move while adding verified Axis2D WASD movement, `F` interaction, `B/R` build controls, `SpaceBar` jump, and `E` dive-entry attempt through `BP_OceanMVPPlayerController`. |
| Survival / inventory | `UOceanSurvivalComponent`, `UOceanInventoryComponent`, `Source/Ocean/OceanPrototype/OceanItemTypes.h` | Tracks health-adjacent MVP pressure through stamina, hydration, satiety, stackable resource costs, `FOceanItemStack` slots, recovery item use, and drag/merge/swap inventory semantics. |
| Interaction | `UOceanInteractionComponent`, `UOceanInteractableInterface` | Finds the nearest valid interactable and routes F interaction to resources or later event actors. |
| Build grid | `Source/Ocean/OceanPrototype/OceanBuildGridComponent.*` | Stable logical cells, footprint rotation, overlap, adjacency checks, and edge-cell detection for water-only actions. |
| Build placement | `Source/Ocean/OceanPrototype/OceanBuildComponent.*`, `Source/Ocean/OceanPrototype/OceanBuildPlacementTypes.h` | Toggles build mode, rotates previews, consumes inventory cost, auto-resolves the floating platform, and exposes typed placement query reasons before placement spawns actors. |
| HUD/backpack UI | `Source/Ocean/OceanPrototype/UI/OceanHUDRootWidget.*`, `/Game/OceanPrototype/UI/*` | Low-obstruction HUD root and right-side backpack drawer foundation; UI owns open/closed presentation state and dispatches commands while gameplay components own rules. |
| Menu / settings UI | `UOceanMainMenuWidget`, `UOceanPauseMenuWidget`, `UOceanSettingsWidget`, `UOceanSaveSlotWidget` | Main-menu and pause-menu command surfaces; widgets bind delegates after C++ WidgetTree construction, save settings through `UOceanSaveManager`, and keep settings overlays above menu/pause layers. |
| Day snapshots / save-load | `UOceanSaveGame`, `UOceanSaveManager`, `AOceanMVPGameMode::AdvanceTimeOfDay` | Stores day/time/event/stats/inventory/platform/settings snapshots; pause UI selects the next snapshot slot, while actual gameplay saves only when a new day starts. |
| Module definition | `Source/Ocean/OceanPrototype/OceanBuildModuleDefinition.*` | Data-asset contract for footprint, cost, preview mesh, and actor class. |
| Module actor | `Source/Ocean/OceanPrototype/OceanBuildModuleActor.*` | Cube-placeholder build module with inherited buoyancy path. |
| Floating platform | `Source/Ocean/OceanPrototype/OceanFloatingPlatform.*` | Owns stable grid and visual bobbing root. |
| Resource node | `Source/Ocean/OceanPrototype/OceanResourceNode.*` | Collectable floating resource actor with cube placeholder and buoyancy. |
| Resource field | `Source/Ocean/OceanPrototype/OceanResourceField.*` | Owns PCG component and deterministic fallback spawn positions. |
| Debug HUD | `Source/Ocean/OceanPrototype/OceanSurvivalHUD.*` | Draws survival stats, inventory counts, build state, and nearest interaction prompt for MVP debugging. |
| MVP map content | `scripts/setup_mvp_survival_loop.py`, `scripts/verify_mvp_survival_loop.py` | Generates and verifies `L_WaterOcean` starter platform, resource field, input assets, Blueprint classes, and the 1x1 deck data asset. |
| Paper2D visual layer | `AOceanCharacter::Paper2DVisualComponent`, `UOceanPaper2DAnimationComponent`, `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe/*` | Adds a camera-facing Paper2D Flipbook presentation layer and lightweight visual state machine to `BP_OceanSurvivorCharacter` without replacing capsule, movement, survival, inventory, interaction, or build components. |

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
2. `IMC_OceanMVP` binds WASD, F, B, R, SpaceBar, E, Tab, I, left mouse, and touch into one Enhanced Input context; W/A/S use Enhanced Input modifiers so W=(0,+1), A=(-1,0), S=(0,-1), D=(+1,0).
3. Floating `AOceanResourceNode` actors expose F pickup and feed `UOceanInventoryComponent`.
4. SpaceBar calls the normal Character jump path; E remains a water-edge dive-entry gate, while raw X toggles the playable MVP dive state when the inventory contains `dive_suit`.
5. X dive moves the character to an underwater floor/fallback depth, switches to walking for seabed traversal, shortens the camera spring arm, and pressing X again returns to water surface while restoring the original arm length.
6. Build mode consumes inventory resources to place `AOceanBuildModuleActor` cells adjacent to the starter platform.
7. `BP_OceanMVPPlayerController` creates `WBP_OceanHUDRoot` through `HUDRootWidgetClass`; Tab/I toggles the backpack drawer while B remains build mode.

### HUD / Backpack Command Boundary

1. `UOceanHUDRootWidget` keeps the drawer open/closed state and logs the initial PIE state as `[TDD] OceanHUDRootPIE: created=1 drawer_open=0`.
2. WBP assets live under `/Game/OceanPrototype/UI`; `WBP_OceanHUDRoot` is parented to `UOceanHUDRootWidget`, while panels, slots, drag visual, placement overlay, modal, and toast stack are `UserWidget` children.
3. Left-click on occupied backpack slots remains drag/drop; right-click calls `UOceanBackpackSlotWidget::RequestUse`, which routes through the owning `UOceanBackpackPanelWidget` into `UOceanInventoryComponent::TryUseItemAtSlot`.
4. Inventory mutation remains in `UOceanInventoryComponent`; survival recovery remains in `UOceanSurvivalComponent`.
5. Widgets may dispatch commands such as use item, move/merge slot, toggle drawer, or request placement preview, but they do not own gameplay rules.
6. Build placement UI must call `QuerySelectedModulePlacement` and read `FOceanPlacementQueryResult` before any final placement command; widgets must not directly spawn build actors.

### Menu / Settings Command Boundary

1. `AOceanPlayerController::BeginPlay` treats `L_MainMenu` as UI-only and creates `UOceanMainMenuWidget` at `ZOrder=30`; game maps create the HUD and allow gameplay input.
2. C++-constructed menu widgets create controls in `RebuildWidget`, then bind button/slider delegates in `NativeConstruct`; binding in `NativeOnInitialized` is too early for these runtime-built trees.
3. `UOceanSettingsWidget` is a shared overlay for main menu and pause menu; it stores BGM/SFX values through `UOceanSaveManager` but does not yet apply audio-mix runtime volume.
4. Settings overlays open at `ZOrder=60`, above the main menu and pause menu, and their root `CanvasPanelSlot` is centered to avoid top-left/default-canvas placement.
5. `UOceanSaveSlotWidget` owns only slot-row UI and emits load/delete delegates; main menu interprets load/delete, while pause menu reuses the load delegate as "select day-snapshot slot".
6. `U` pause input is game-map only; pressing it on `L_MainMenu` is ignored so the start menu cannot stack a pause menu over itself.

### Save / Load Snapshot Boundary

1. `UOceanSaveManager::SetActiveSnapshotSlot` selects which of the three supported slots receives the next day snapshot; the pause menu does not write an immediate manual save.
2. `AOceanMVPGameMode::AdvanceTimeOfDay` calls `SaveDaySnapshotToSlot` only when Night rolls into the next Morning after incrementing `CurrentDay`.
3. `UOceanSaveGame` stores day, time-of-day, total events, events processed today, event timer, game phase, autoplay seed, survival stats, resource stacks, item slots, platform location, and audio settings.
4. `UOceanInventoryComponent::RestoreInventoryState` is the restore boundary for saved resources and item slots; UI widgets must not edit saved arrays directly.
5. Loading forces `AOceanCharacter` to `ForceSurfaceAtSafeLocation` near the saved platform, clears dive state, restores walking movement, and resets the camera to the surface arm length.
6. Day snapshots are coarse progress checkpoints; they do not persist exact underwater position or mid-event camera state.

### Paper2D Presentation

1. Paper2D is enabled as a project plugin and runtime dependency of `Ocean`.
2. `AOceanCharacter` owns `Paper2DVisualComponent` and `OceanPaper2DAnimationComponent`; gameplay remains on the existing Ocean pawn and components.
3. `scripts/import_paper2d_experiment.py` imports the external `288x288` experiment frames as Textures, Sprites, and Flipbooks under `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe`.
4. `scripts/setup_mvp_survival_loop.py` assigns all 24 imported Flipbooks to `OceanPaper2DAnimationComponent`.
5. Each character tick updates only the Paper2D visual: it faces the active camera and chooses `Idle`, `Walk`, or `Jump` from movement state; `Swim`, `Climb`, and `DiveSuitDive` are assignable visual states for future event systems.

### Resource Spawn

1. PCG is the preferred authoring path for scatter rules.
2. `AOceanResourceField` provides deterministic fallback locations for tests and early maps.
3. Spawned `AOceanResourceNode` actors float on the water and return `FOceanResourceStack` when collected.

## 4. Plugin Boundaries

| Plugin | Role | Boundary |
|---|---|---|
| Water | Ocean surface, water collision profile, buoyancy sampling. | Required for water actors and `UBuoyancyComponent`. |
| PCG | Resource scattering and future island/loot generation. | Runtime actors can provide deterministic fallback until PCG graph assets exist. |
| Paper2D | HD2D player presentation through sprites, flipbooks, and a lightweight visual state machine. | Visual layer only; it must not own movement, survival, inventory, interaction, or build authority. |
| UnrealBridge | Editor automation, Python execution, PIE/log capture, save gate. | Editor-only; never required by packaged runtime. |

## 5. Prototype Rules

- Module is a group of grid cells, not necessarily one cell.
- Logical grid is stable even when visuals bob on waves.
- Missing models use cube placeholders first.
- Floating actor assets must retain buoyancy components.
- Water setup is incomplete unless `WaterBodyCollision` exists in `Config/DefaultEngine.ini`.
- Fishing minigame, oxygen, underwater collection rewards, island travel, and cruise intro remain out of MVP scope until the small-boat loop is playable and testable; the current X dive is a minimal seabed/camera traversal slice, not complete underwater gameplay.
- HUD/backpack work is currently UI/WBP/interaction infrastructure only: not final art, not full drawer animation, not fishing/diving/island-event UI, and not final item icons.
