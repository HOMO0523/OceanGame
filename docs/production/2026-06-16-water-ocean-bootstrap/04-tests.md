---
unit_id: 2026-06-16-water-ocean-bootstrap
status: verified
owner: test-designer
updated_at: 2026-06-16T17:40:00
source_commit: working-tree
depends_on: [2026-06-16-automation-migration]
parallel_lock: Ocean.WaterSetup
---

# Tests

| ID | Test | Method | Expected |
|---|---|---|---|
| W-001 | Water profile exists | `rg -n "WaterBodyCollision" Config/DefaultEngine.ini` | One profile entry exists. |
| W-002 | Script parses | `python -m py_compile scripts/create_water_ocean_map.py` | Exit `0`. |
| W-003 | Bridge available | `python scripts/ue_tdd_pipeline.py --check-only` | Connects to bridge, or reports blocker. |
| W-004 | UE Python remote available | `remote_execution.py` probe | Nodes found, or reports missing fallback. |
| W-005 | Map create/save | Run script inside UE | `[TDD] WaterOceanSave: result=PASS`. |
| W-006 | Map actor counts | Script verification logs | `[TDD] WaterOceanActorCount: actual=1 expected=1` and `WaterZoneActorCount: actual=1 expected=1`. |
| W-013 | Landmass plugin configured | `rg -n "Landmass" Ocean.uproject` | Landmass is explicitly enabled. |
| W-014 | Support Landscape exists | Run script inside UE | `[TDD] OceanSupportLandscapeCount: actual=1 expected=1`. |
| W-015 | Support Landscape initialized | Run script inside UE | `[TDD] OceanSupportLandscapeComponentCount: actual=256 expected=256`. |
| W-016 | Water brush manager exists | Run script inside UE | `[TDD] OceanWaterBrushManagerCount: actual=1 expected=1`. |
