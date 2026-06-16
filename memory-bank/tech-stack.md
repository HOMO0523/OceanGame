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
| UnrealBridge | Editor-only Python/TCP bridge for automation, PIE/log capture, asset creation, save gate. |
| EditorScriptingUtilities | Editor-only Python asset helpers used by `scripts/create_water_ocean_map.py`. |

`UnrealBridge` declares editor plugin dependencies on `PythonScriptPlugin`, `GameplayAbilities`, `EnhancedInput`, `PoseSearch`, `Chooser`, and `StructUtils`. UE 5.7 warns that `StructUtils` is deprecated, but the copied bridge source currently links its module.

## Planned Presentation Stack

| Technology | Purpose | Boundary |
|---|---|---|
| Paper2D | Planned player-character visual layer for the HD2D look. | Not yet enabled as a verified project plugin in this docs step; when implemented, it should attach Sprite/Flipbook visuals to `BP_OceanSurvivorCharacter` without replacing the Ocean gameplay Pawn. |
| HD2D styling | 2D character over 3D water/platform scenes with high saturation, lighting/post-process, and fixed 45-60 degree top-down camera. | Visual style only; collision, survival, interaction, inventory, and build systems stay in existing Ocean gameplay components. |

## Project Scripts

| Script | Purpose |
|---|---|
| `scripts/ue_tdd_bridge.py` | TCP client for UnrealBridge discovery, Python execution, PIE control, and log capture. |
| `scripts/ue_tdd_pipeline.py` | no-LiveCoding save → close → build → launch → PIE → capture → analyze loop for `OceanEditor`. |
| `scripts/harness_state_validator.py` | Validates `docs/production` state folders, metadata, schemas, and `parallel_lock` collisions. |
| `scripts/doc_sync_hook.py` | Manual/pre-commit documentation drift detector and progress snapshot updater. |
| `scripts/create_water_ocean_map.py` | Editor Python script that creates `/Game/OceanPrototype/Maps/L_WaterOcean` with WaterZone, WaterBodyOcean, support Landscape, and WaterBrushManager. |

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
```

`--check-only` reads logs only; it is not compile proof.
