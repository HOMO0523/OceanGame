---
unit_id: 2026-06-16-water-ocean-bootstrap
status: verified
owner: infra-auditor
updated_at: 2026-06-16T17:10:00
source_commit: working-tree
depends_on: [2026-06-16-automation-migration]
parallel_lock: Ocean.WaterSetup
---

# Infra Audit

decision: needs-infra

| Requirement | Status | Evidence | Needed Change |
|---|---|---|---|
| Water plugin | supported | `Ocean.uproject` enables `Water`; editor log mounted Water. | None. |
| Water collision profile | supported | `Config/DefaultEngine.ini` has `WaterBodyCollision`; editor was restarted before map creation. | None. |
| Water classes | supported | UE source exposes `AWaterZone` and `AWaterBodyOcean`. | Use `/Script/Water.WaterZone` and `/Script/Water.WaterBodyOcean`. |
| Editor scripting asset creation | supported | `EditorScriptingUtilities` enabled; `scripts/create_water_ocean_map.py` executed after restart. | None. |
| UnrealBridge | supported | Relaunched editor reported Bridge ready on port `55922`. | None. |
| UE Python remote execution | not required | Remote nodes were unavailable; UnrealBridge handled execution. | Keep using bridge for this workflow. |
| Safe editor shutdown | supported | Save gate returned `save_result=True dirty_before=[] dirty_after=[]`; editor closed and cold build ran. | None. |
| Second editor/commandlet write | guarded | No second concurrent editor write was used; creation ran inside the relaunched editor. | Keep this guard. |

## Boundary Analysis

### 1. Editor State Boundary

The initial editor process was launched before `UnrealBridge` and `EditorScriptingUtilities` were enabled. The corrected chain was: save dirty packages through UnrealBridge, close the editor, run `OceanEditor Win64 Development -NoHotReload`, then reopen `Ocean.uproject` with `-UnrealBridgeForceReady`.

### 2. Asset Authority Boundary

`L_WaterOcean` is a binary `.umap` asset. It must be created through Unreal Editor APIs, not by hand-editing files. Any script that writes it must save packages and verify the asset exists afterward.

### 3. Water Plugin Boundary

Water setup is not just a mesh. The accepted water body is `AWaterBodyOcean` inside an `AWaterZone`, backed by the Water plugin and `WaterBodyCollision` profile. A static plane is a visual placeholder only and is forbidden for this unit.

### 4. Build Grid Boundary

The first ocean map must not make the construction grid wave-relative. Future platform visuals may bob, but the placement grid remains stable in world/logical coordinates.

### 5. PCG Boundary

PCG resource scattering is not part of this map bootstrap. The water map may host future `AOceanResourceField`, but this unit should not invent a fake PCG graph before the scatter contract is designed.

### 6. Verification Boundary

`Ocean Win64` build success proves runtime code was not broken; it does not prove the map exists. For this unit, the accepted verification is the editor-target cold build plus bridge-run `[TDD]` map creation logs and the saved `Content/OceanPrototype/Maps/L_WaterOcean.umap` file.
