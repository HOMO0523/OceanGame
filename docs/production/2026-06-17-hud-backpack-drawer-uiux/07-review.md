---
unit_id: 2026-06-17-hud-backpack-drawer-uiux
status: approved
owner: reviewer
updated_at: 2026-06-17T16:10:00+08:00
source_commit: 76ddd07
depends_on: [2026-06-16-minimal-loop-workflow]
parallel_lock: Ocean.UI.InventoryPlacement
---

# Review

| Severity | Finding | File/Line or Evidence | Required Fix |
|---|---|---|---|
| Medium | Runtime UI does not exist yet. | Production unit is planning-only. | Execute the approved implementation plan. |
| Medium | Slot inventory model is missing. | `UOceanInventoryComponent` is resource-stack only. | Add item slots and drag/drop tests. |
| Medium | Placement has generic failure text. | Build component currently returns a generic invalid placement message. | Add typed placement query result. |

## Missing Tests

- `Ocean.MVP.Inventory.UseItem`
- `Ocean.MVP.Inventory.DragDropModel`
- `Ocean.MVP.Build.PlacementQuery`
- `Ocean.MVP.UI.HUDStateModel`
- UnrealBridge UI asset and PIE probes

## Boundary / Dependency Risks

- UI must not mutate inventory or spawn actors directly.
- The `B` build key must remain functional after adding backpack input.
- WBP asset generation must save dirty packages before editor close.

## Stale Document Risks

- `docs/defense/index.html` must be updated after runtime verification, not before.
- `memory-bank/progress.md` should only claim WBP completion once assets load in PIE.

## Readiness Decision

readiness: fix

This unit is ready for implementation planning and execution, but not runtime shipping.
