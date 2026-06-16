---
unit_id: 2026-06-16-mvp-survival-loop
status: planned
owner: planner
updated_at: 2026-06-16T18:52:00
source_commit: 065269f
depends_on: [2026-06-16-water-ocean-bootstrap]
parallel_lock: Ocean.MVPSurvivalLoop
---

# Tests

## C++ Automation Tests

- `Ocean.MVP.Input.CameraRelativeMove`: WASD vector converts to flattened camera-relative world direction.
- `Ocean.MVP.Inventory.Stacking`: stack inventory adds, removes, rejects insufficient cost, and reports full capacity.
- `Ocean.MVP.Survival.Tick`: hydration/satiety drain and stamina recovery clamp to expected values.
- `Ocean.MVP.Interaction.Priority`: interaction component chooses nearest valid interactable and reports failure when none exists.
- `Ocean.MVP.Build.DeckPlacement`: deck placement spends wood and reserves one adjacent grid cell.

## Editor / PIE Checks

- `scripts/setup_mvp_survival_loop.py` saves generated input assets, blueprint assets, and `L_WaterOcean`.
- `scripts/verify_mvp_survival_loop.py` emits `[TDD]` lines:
  - `MVPMapLoaded: PASS`
  - `MVPPlayerStartExists: PASS`
  - `MVPSurvivorBlueprintExists: PASS`
  - `MVPInputMappingExists: PASS`
  - `MVPPlatformExists: PASS`
  - `MVPResourceFieldExists: PASS`
  - `MVPBuoyantResourceCount actual>=1`
  - `MVPBuoyantModuleClass: PASS`

## Required Commands

```powershell
python scripts/harness_state_validator.py --json
python -m py_compile scripts/setup_mvp_survival_loop.py scripts/verify_mvp_survival_loop.py
python scripts/ue_tdd_pipeline.py --pie-duration 5
git diff --check
```

