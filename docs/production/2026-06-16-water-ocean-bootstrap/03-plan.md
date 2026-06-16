---
unit_id: 2026-06-16-water-ocean-bootstrap
status: updated
owner: planner
updated_at: 2026-06-16T16:50:00
source_commit: working-tree
depends_on: [2026-06-16-automation-migration]
parallel_lock: Ocean.WaterSetup
---

# Plan

## Files

- Modify `Ocean.uproject` to enable `EditorScriptingUtilities`.
- Create `scripts/create_water_ocean_map.py`.
- Create this production unit.
- Update `memory-bank/progress.md` after verification/blocker classification.

## Execution Order

1. Confirm bridge and Python remote status.
2. Write a deterministic WaterOcean creation script.
3. Verify script syntax outside UE.
4. If editor automation is available, execute the script and save.
5. If editor automation is blocked, do not kill the editor; record blocker and provide exact run command.

## Done Criteria

- Script exists and compiles.
- Water boundary analysis is documented.
- If safe automation is available, `/Game/OceanPrototype/Maps/L_WaterOcean` exists and contains one WaterZone and one WaterBodyOcean.
- If safe automation is blocked, the blocker is explicit and no unsafe editor action is taken.
