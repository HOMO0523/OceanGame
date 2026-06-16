---
unit_id: 2026-06-16-minimal-loop-workflow
status: designed
owner: gpt
updated_at: 2026-06-17T00:30:00+08:00
source_commit: 0964ce7
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.MinimalLoopWorkflow
---

# Tests

## Planned Code-Stage Tests

- `BridgeVerifyOceanMoveAxis2D`: W/S/A/D mappings produce four intended Axis2D values, not the same right vector.
- `[TDD] OceanMoveInputVector`: PIE logs input vector and world direction for movement actions.
- `BridgeVerifyOceanPaper2DCharacter`: `BP_OceanSurvivorCharacter` keeps Ocean components and owns a Paper2D visual component.
- `Ocean.MVP.Visual.FlipbookDirectionModel`: movement direction selects the expected 4/8-direction Paper2D animation key.
- `Ocean.MVP.Survival.RecoveryItems`: food and water recover stats, clamp at max, and fail cleanly when absent.
- `Ocean.MVP.Survival.DeathThreshold`: zero hydration/satiety progresses toward failure deterministically.
- `Ocean.MVP.Inventory.UseItem`: item use consumes inventory and applies gameplay effect atomically.
- `Ocean.MVP.Inventory.DragDropModel`: merge, swap, split, and invalid drag outcomes never lose items.
- `Ocean.MVP.Crafting.Recipes`: recipes validate cost, output, and failure without partial spend.
- `Ocean.MVP.Build.AllowedArea`: placement rejects blocked, detached, occupied, and out-of-radius cells.
- `Ocean.MVP.Events.AdvanceTime`: event points, day count, seeded random event selection, and status drain match expected values.
- `Ocean.MVP.Boat.DriftProgress`: direction/rest/event choices advance or preserve drift progress by rule.
- `Ocean.MVP.Ending.SurviveSevenDays`: 21 event points while alive triggers coast ending.

## Docs-Only Verification

```powershell
python scripts/harness_state_validator.py --json
python scripts/doc_sync_hook.py --phase manual --history
git diff --check
```
