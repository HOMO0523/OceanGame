# Technology Stack — Ocean

## Engine And Core

- Unreal Engine 5.7 (`EngineAssociation=5.7`)
- C++ runtime module: `Ocean`
- C++ editor module: `OceanEditor`
- Primary platform: Windows x64
- Default RHI: DX12 / SM6
- Rendering baseline: Lumen, Substrate, ray tracing project settings inherited from template

## Enabled Plugins

| Plugin | Purpose |
|---|---|
| ModelingToolsEditorMode | Editor modeling tools for placeholder geometry and quick blockouts. |
| StateTree | Template gameplay support. |
| GameplayStateTree | Template gameplay support. |
| Water | Water bodies, water collision profile, buoyancy support. |
| PCG | Procedural resource scattering. |
| Landmass | Terrain/brush support for WaterBodyOcean shoreline and landscape deformation workflows. |
| Paper2D | Experimental HD2D player presentation through sprites and flipbooks. |
| UnrealBridge | Editor-only Python/TCP bridge for automation, PIE/log capture, asset creation, save gate. |
| EditorScriptingUtilities | Editor-only Python asset helpers used by `scripts/create_water_ocean_map.py`. |

`UnrealBridge` declares editor plugin dependencies on `PythonScriptPlugin`, `GameplayAbilities`, `EnhancedInput`, `PoseSearch`, `Chooser`, and `StructUtils`. UE 5.7 warns that `StructUtils` is deprecated, but the copied bridge source currently links its module.

## Presentation Stack

| Technology | Purpose | Boundary |
|---|---|---|
| UMG / Widget Blueprint | Low-obstruction HUD and right-side backpack drawer foundation under `/Game/OceanPrototype/UI`. | WBP layer is presentation and command dispatch only; gameplay rules stay in C++ components. |
| Paper2D | Active experimental player-character visual layer for the HD2D look. | Enabled as a project plugin and `Ocean` runtime dependency; attaches a Flipbook component and lightweight animation component to `BP_OceanSurvivorCharacter` without replacing the Ocean gameplay Pawn. |
| HD2D styling | 2D character over 3D water/platform scenes with high saturation, lighting/post-process, and fixed 45-60 degree top-down camera. | Visual style only; collision, survival, interaction, inventory, and build systems stay in existing Ocean gameplay components. |

## HUD / UI Runtime

| Runtime Type / Asset | Purpose |
|---|---|
| `UOceanHUDRootWidget` | C++ root widget state model for the backpack drawer; logs real PIE creation state. |
| `WBP_OceanHUDRoot` | Widget Blueprint child of `UOceanHUDRootWidget`; bound as the MVP player controller HUD class. |
| `WBP_StatusPanel`, `WBP_TimePanel`, `WBP_TopRightPanel` | Low-obstruction HUD panel placeholders. |
| `WBP_BackpackDrawer`, `WBP_InventorySlot`, `WBP_ItemDragVisual` | Right-side backpack drawer and drag/drop presentation placeholders. |
| `WBP_PlacementOverlay` | Build-placement feedback placeholder that should consume placement query results. |
| `WBP_ConfirmModal`, `WBP_ToastStack` | Common UI feedback placeholders. |

## Project Scripts

| Script | Purpose |
|---|---|
| `scripts/ue_tdd_bridge.py` | TCP client for UnrealBridge discovery, Python execution, PIE control, and log capture. |
| `scripts/ue_tdd_pipeline.py` | no-LiveCoding save → close → build → launch → PIE → capture → analyze loop for `OceanEditor`. |
| `scripts/harness_state_validator.py` | Validates `docs/production` state folders, metadata, schemas, and `parallel_lock` collisions. |
| `scripts/doc_sync_hook.py` | Manual/pre-commit documentation drift detector and progress snapshot updater. |
| `scripts/create_water_ocean_map.py` | Editor Python script that creates `/Game/OceanPrototype/Maps/L_WaterOcean` with WaterZone, WaterBodyOcean, support Landscape, and WaterBrushManager. |
| `scripts/import_paper2d_experiment.py` | Editor Python script that imports the external `288x288` Paper2D experiment frames as Textures, Sprites, and Flipbooks under `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe`. |
| `scripts/create_ocean_ui_assets.py` | Editor Python script that creates and saves the 10 HUD/backpack Widget Blueprint assets under `/Game/OceanPrototype/UI`. |
| `scripts/verify_ocean_ui_assets.py` | UnrealBridge verification for WBP asset load, HUDRootWidgetClass binding, map load, and recent real PIE HUD log presence. |
| `scripts/verify_ocean_ui_pie.py` | External Bridge client probe that loads `L_WaterOcean`, starts PIE, waits client-side so the editor can tick, and verifies `[TDD] OceanHUDRootPIE: created=1`. |
| `scripts/verify_main_menu_navigation.py` | External Bridge client probe that starts PIE on `L_MainMenu`, verifies `NewGameButton` is bound, broadcasts the click, and confirms travel to `L_WaterOcean`. |

## Gameplay Input Assets

| Input Asset | Default Key | Purpose |
|---|---|---|
| `IA_OceanMove` | `W/A/S/D`, left mouse, touch | Corrected Axis2D movement plus preserved click/touch movement. |
| `IA_OceanInteract` | `F` | Unified nearby interaction. |
| `IA_OceanToggleBuild` | `B` | Build mode toggle. |
| `IA_OceanToggleBackpack` | `Tab`, `I` | Right-side backpack drawer toggle through `UOceanHUDRootWidget`; closes back to game input. |
| `IA_OceanRotateBuild` | `R` | Build preview rotation. |
| `IA_OceanJump` | `SpaceBar` | Character jump using `ACharacter::Jump()`. |
| `IA_OceanDive` | `E` | Water-edge dive-entry attempt; not full underwater gameplay. |

## Paper2D Animation Runtime

| Runtime Type | Purpose |
|---|---|
| `UPaperFlipbookComponent` | Plays the currently selected imported sprite animation. |
| `UOceanPaper2DAnimationComponent` | Keeps the Flipbook visual facing the active camera and selects state/direction Flipbooks. |
| `EOceanPaper2DAnimationState` | Visual states: `Idle`, `Walk`, `Jump`, `Swim`, `Climb`, `DiveSuitDive`. |
| `EOceanPaper2DDirection` | Cardinal visual directions: `South`, `West`, `East`, `North`. |

## Editor Automation Library

| Symbol | Purpose |
|---|---|
| `UOceanLandscapeAutomationLibrary::EnsureWaterOceanLandscapeSupport` | Editor-only helper exposed to Python as `unreal.OceanLandscapeAutomationLibrary.ensure_water_ocean_landscape_support()`. Creates `Landscape_WaterSupport` through `ALandscape::Import`, verifies 256 components, and spawns `WaterBrushManager_Prototype`. |

## Local Skills

| Skill | Location | Purpose |
|---|---|---|
| `ue-tdd-livecoding` | `.agents/skills/ue-tdd-livecoding/SKILL.md` | Historical name; actual no-LiveCoding UE TDD workflow. |
| `ocean-ai-production-hardness` | `.agents/skills/ocean-ai-production-hardness/SKILL.md` | Ocean-focused production loop for Water, build grid, PCG, buoyancy, and placeholder assets. |

## Build Commands

```powershell
& "{UE_ROOT}\Engine\Build\BatchFiles\Build.bat" Ocean Win64 Development "{ProjectRoot}\Ocean.uproject" -NoHotReload
& "{UE_ROOT}\Engine\Build\BatchFiles\Build.bat" OceanEditor Win64 Development "{ProjectRoot}\Ocean.uproject" -NoHotReload
```

## TDD Pipeline

```powershell
python scripts/ue_tdd_pipeline.py --pie-duration 5
python scripts/ue_tdd_pipeline.py --check-only
python scripts/ue_tdd_pipeline.py --pre-pie-console-command "stat fps" --pie-duration 5
python scripts/verify_ocean_ui_pie.py
python scripts/verify_main_menu_navigation.py
```

`--check-only` reads logs only; it is not compile proof.
For gameplay-map HUD PIE validation, prefer `scripts/verify_ocean_ui_pie.py` because Unreal Python sleeps block editor ticks inside a single Bridge execution.
For main-menu navigation validation, use `scripts/verify_main_menu_navigation.py`; it checks the runtime UMG delegate before testing the `New Game` level travel.
