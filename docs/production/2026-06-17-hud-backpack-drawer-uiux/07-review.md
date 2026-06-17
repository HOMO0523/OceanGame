---
unit_id: 2026-06-17-hud-backpack-drawer-uiux
status: reviewed
owner: documentation-sync
updated_at: 2026-06-17T19:15:00+08:00
source_commit: 1f8068e
depends_on: [2026-06-16-minimal-loop-workflow]
parallel_lock: Ocean.UI.InventoryPlacement
---

# Review

| Severity | Finding | File/Line or Evidence | Required Fix |
|---|---|---|---|
| Info | Runtime UI foundation now exists and is PIE verified. | `1f8068e`, `f2bb106`, and `[TDD] OceanHUDRootPIE: created=1 drawer_open=0`. | None for this infrastructure slice. |
| Info | Slot inventory model, use-item recovery, and drag/drop are implemented. | `8738a06`, `ed21f77`, `76ba3dc`. | Continue with final icon/data work in a later UI polish slice. |
| Info | Placement now exposes query reasons before actor spawn. | `2c70e1f`; UI can consume `FOceanPlacementQueryResult`. | Keep UI as command dispatch; do not spawn actors directly from widget code. |

## Passing / Covered Tests

- `Ocean.MVP.Inventory.UseItem`
- `Ocean.MVP.Inventory.DragDropModel`
- `Ocean.MVP.Build.PlacementQuery`
- `Ocean.MVP.UI.HUDStateModel`
- UnrealBridge WBP asset probe
- Real PIE HUD root probe through `python scripts/ue_tdd_pipeline.py --pie-duration 5 --log-lines 24000`
- Strict `scripts/verify_ocean_ui_assets.py --pie` recent-log check with `real_created_log=1`

## Boundary / Dependency Status

- UI remains a presentation and command-dispatch layer.
- Inventory and survival rules stay in gameplay components.
- Drag-to-place must use placement query first rather than direct widget-driven actor spawn.
- `B` build key remains separate from `Tab` / `I` backpack input.
- WBP asset generation and setup scripts run the editor save gate.

## Remaining Product Scope

- Final art is not complete.
- The right-side drawer is an interaction foundation, not a fully animated final drawer.
- Fishing, full diving, island travel, and event UI are deferred.
- Item icons remain placeholder-ready and are not final replacements.

## Readiness Decision

readiness: pass

This unit is ready as a verified UI/WBP/interaction infrastructure foundation. Treat the next work as visual polish and content expansion, not as a repair of the foundation.
