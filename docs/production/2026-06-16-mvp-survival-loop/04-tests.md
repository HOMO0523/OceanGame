---
unit_id: 2026-06-16-mvp-survival-loop
status: implemented
owner: planner
updated_at: 2026-06-16T23:50:00
source_commit: 3cfc434
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
- `Ocean.MVP.Build.AutoFindsTargetPlatform`: build placement succeeds after auto-resolving the map's floating platform when no target platform was explicitly assigned.
- `Ocean.MVP.Build.FailureCases`: build placement rejects inactive mode, missing module, unaffordable cost, occupied cells, detached cells, and destroyed target platform.

## Editor / PIE Checks

- `scripts/setup_mvp_survival_loop.py` saves generated input assets, blueprint assets, and `L_WaterOcean`.
- `scripts/verify_mvp_survival_loop.py` emits `[TDD]` lines:
  - `MVPMapLoaded: result=PASS`
  - `MVPInputContext mappings=9 expected=9 result=PASS`
  - `MVPPlayerControllerInput result=PASS`
  - `MVPSurvivorBuildSelectedModule result=PASS`
  - `MVPGameModePlayerController result=PASS`
  - `MVPPlatformCount actual=1 expected>=1`
  - `MVPResourceFieldCount actual=1 expected>=1`
  - `MVPStarterResourceNodeCount actual=16 expected=16 result=PASS`
  - `MVPDeckModuleActorClass result=PASS`

## Required Commands

```powershell
python scripts/harness_state_validator.py --json
python -m py_compile scripts/setup_mvp_survival_loop.py scripts/verify_mvp_survival_loop.py
python scripts/ue_tdd_pipeline.py --no-launch
& "D:\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "D:\UE5 demo\Ocean\Ocean.uproject" -Unattended -NullRHI -NoSplash -NoSound -NoLiveCoding -ExecCmds="Automation RunTests Ocean.MVP.Build" -TestExit="Automation Test Queue Empty"
git diff --check
```
