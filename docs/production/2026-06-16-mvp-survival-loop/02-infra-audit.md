---
unit_id: 2026-06-16-mvp-survival-loop
status: audited
owner: planner
updated_at: 2026-06-16T18:52:00
source_commit: 065269f
depends_on: [2026-06-16-water-ocean-bootstrap]
parallel_lock: Ocean.MVPSurvivalLoop
---

# Infrastructure Audit

| Requirement | Status | Evidence | Gap |
|---|---|---|---|
| Water map | supported | `/Game/OceanPrototype/Maps/L_WaterOcean` exists with WaterBodyOcean, WaterZone, support Landscape, WaterBrushManager. | Add gameplay actors to this map. |
| Water collision | supported | `WaterBodyCollision` is present in `Config/DefaultEngine.ini`. | Recheck during final validation. |
| Build grid | supported | `UOceanBuildGridComponent` supports cell conversion, footprints, adjacency, reservation. | Add player-facing placement component/UI. |
| Module actor | supported | `AOceanBuildModuleActor` has placeholder mesh and `UBuoyancyComponent`. | Add default deck definition asset and spawn path. |
| Resource node | partial | `AOceanResourceNode` can be collected and has buoyancy. | Add interaction interface and inventory transfer. |
| Resource field | partial | `AOceanResourceField` owns `UPCGComponent` and fallback spawn logic. | Place/configure field in `L_WaterOcean`. |
| Survival state | missing | No Ocean survival component exists. | Add `UOceanSurvivalComponent` plus tests. |
| Inventory | missing | Only `FOceanResourceStack` exists. | Add stack inventory component plus tests. |
| WASD movement | partial | TopDown controller exists; TwinStick/Strategy examples show Enhanced Input movement. | Add Ocean-specific move action while retaining click path. |
| `F` interaction | missing | No unified interaction interface/component exists. | Add interface, scanner, resource pickup implementation. |
| HUD | missing | No Ocean HUD exists. | Add debug-quality HUD to show status, inventory, prompts, build mode. |
| Editor automation | supported | UnrealBridge and no-LiveCoding pipeline are available. | Add setup/verification scripts for gameplay assets/map actors. |

