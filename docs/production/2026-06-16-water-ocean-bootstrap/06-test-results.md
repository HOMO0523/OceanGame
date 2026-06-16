---
unit_id: 2026-06-16-water-ocean-bootstrap
status: verified
owner: test-runner
updated_at: 2026-06-16T17:40:00
source_commit: working-tree
depends_on: [2026-06-16-automation-migration]
parallel_lock: Ocean.WaterSetup
---

# Test Results

result: pass
failure_type: none
repro_command: `python scripts/create_water_ocean_map.py` executed through UnrealBridge after save/close/build/open
observed: map saved, map exists, one WaterBodyOcean, one WaterZone, one support Landscape with 256 components, and one WaterBrushManager
expected: bridge-run editor script creates and verifies `/Game/OceanPrototype/Maps/L_WaterOcean` with Water/Landmass support
return_gate: none

## Results

| ID | Result | Evidence |
|---|---|---|
| W-001 | PASS | `Config/DefaultEngine.ini` contains `WaterBodyCollision`. |
| W-002 | PASS | `python -m py_compile scripts/create_water_ocean_map.py ...` exited `0`. |
| W-003 | PASS | Relaunched editor accepted UnrealBridge execution on port `55922`. |
| W-004 | PASS | Remote execution was not required; bridge execution replaced it. |
| W-005 | PASS | `[TDD] WaterOceanSave: result=PASS` and `[TDD] WaterOceanMapExists: result=PASS`. |
| W-006 | PASS | `[TDD] WaterOceanActorCount: actual=1 expected=1` and `[TDD] WaterZoneActorCount: actual=1 expected=1`. |
| W-007 | PASS | `Build.bat Ocean Win64 Development ... -NoHotReload` returned `Result: Succeeded`. |
| W-008 | PASS | `Build.bat OceanEditor Win64 Development ... -NoHotReload` returned `Result: Succeeded`. |
| W-009 | PASS | `Content/OceanPrototype/Maps/L_WaterOcean.umap` exists on disk. |
| W-010 | PASS | `python scripts/harness_state_validator.py --json` returned `"success": true`. |
| W-011 | PASS | `git diff --check` returned exit `0` with line-ending warnings only. |
| W-012 | PASS | Bridge direct check returned `WaterOceanAssetExists: result=PASS`, `WaterOceanActorCount: actual=1 expected=1`, `WaterZoneActorCount: actual=1 expected=1`, and `world=/Game/OceanPrototype/Maps/L_WaterOcean.L_WaterOcean`. |
| W-013 | PASS | `Ocean.uproject` explicitly enables `Landmass`. |
| W-014 | PASS | `[TDD] OceanSupportLandscapeCount: actual=1 expected=1`. |
| W-015 | PASS | `[TDD] OceanSupportLandscapeComponentCount: actual=256 expected=256`. |
| W-016 | PASS | `[TDD] OceanWaterBrushManagerCount: actual=1 expected=1`. |
| W-017 | PASS | Re-running `scripts/create_water_ocean_map.py` through UnrealBridge returned all Water/Landmass `[TDD]` checks without duplicating actors. |

## Executed Chain

The corrected chain used for this unit:

```powershell
save dirty packages -> close editor -> Build.bat OceanEditor Win64 Development "D:\UE5 demo\Ocean\Ocean.uproject" -NoHotReload -> reopen Ocean.uproject -> execute scripts/create_water_ocean_map.py through UnrealBridge
```

## Crash Triage

The first C++ Landscape helper crashed at `LandscapeEditLayers.Num() == 0` because UE5.7 auto-enables edit layers while registering a new Landscape. The fix was to remove the redundant `CreateDefaultLayer()` call and let `ALandscape::Import` plus registration own the default edit-layer lifecycle.
