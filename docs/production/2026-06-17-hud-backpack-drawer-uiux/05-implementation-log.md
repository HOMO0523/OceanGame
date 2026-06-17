---
unit_id: 2026-06-17-hud-backpack-drawer-uiux
status: implemented
owner: documentation-sync
updated_at: 2026-06-17T19:15:00+08:00
source_commit: 1f8068e
depends_on: [2026-06-16-minimal-loop-workflow]
parallel_lock: Ocean.UI.InventoryPlacement
---

# Implementation Log

Task 9 sync records the completed HUD/backpack drawer UIUX foundation. This unit is implemented and verified as an infrastructure slice, not final UI art.

## Completed Runtime Foundation

| Area | Evidence Commits | Result |
|---|---|---|
| Inventory slot model | `4286c98`, `8738a06` | Added `FOceanItemStack` / `FOceanInventorySlot`, max-stack behavior, and atomic add rollback coverage. |
| Recovery item use | `ed21f77` | Consumable stacks can call `UOceanSurvivalComponent::ApplyRecovery` and consume a slot item. |
| Drag/drop model | `76ba3dc` | `MoveOrMergeSlot` supports reject, merge, swap, and slot reindexing semantics. |
| Placement query reasons | `6945973`, `2c70e1f` | UI-facing placement checks now return typed reasons before any actor spawn path proceeds. |
| HUD root state | `43bad35` | `UOceanHUDRootWidget` owns backpack open/closed state and emits `[TDD] OceanUIToggleBackpack`. |
| Backpack input | `b5a03e8`, `0e8ec78` | `Tab` and `I` toggle the backpack; closing the drawer restores game input and cursor state. |
| WBP assets | `cf5b33c`, `a49405b`, `e7a8f4c` | Created 10 Widget Blueprint assets under `/Game/OceanPrototype/UI`; parent-class verification rejects invalid parents. |
| Real PIE verification | `f2bb106`, `1f8068e` | Asset verification and `--pie` checks require the real `OceanHUDRootPIE` log, not a synthetic success line. |

## WBP Asset Inventory

- `/Game/OceanPrototype/UI/WBP_OceanHUDRoot` parent class: `UOceanHUDRootWidget`.
- `/Game/OceanPrototype/UI/WBP_StatusPanel` parent class: `UserWidget`.
- `/Game/OceanPrototype/UI/WBP_TimePanel` parent class: `UserWidget`.
- `/Game/OceanPrototype/UI/WBP_TopRightPanel` parent class: `UserWidget`.
- `/Game/OceanPrototype/UI/Inventory/WBP_BackpackDrawer` parent class: `UserWidget`.
- `/Game/OceanPrototype/UI/Inventory/WBP_InventorySlot` parent class: `UserWidget`.
- `/Game/OceanPrototype/UI/Inventory/WBP_ItemDragVisual` parent class: `UserWidget`.
- `/Game/OceanPrototype/UI/Build/WBP_PlacementOverlay` parent class: `UserWidget`.
- `/Game/OceanPrototype/UI/Common/WBP_ConfirmModal` parent class: `UserWidget`.
- `/Game/OceanPrototype/UI/Common/WBP_ToastStack` parent class: `UserWidget`.

## Controller Binding

- `BP_OceanMVPPlayerController.HUDRootWidgetClass` is bound to `/Game/OceanPrototype/UI/WBP_OceanHUDRoot.WBP_OceanHUDRoot_C`.
- `BP_OceanMVPPlayerController` owns the HUD root creation path and logs `[TDD] OceanHUDRootPIE: created=1 drawer_open=0` in PIE.
- `IA_OceanToggleBackpack` is mapped through `IMC_OceanMVP` to `Tab` and `I`; build mode remains on `B`.

## Architecture Boundary

- UI is presentation and command dispatch only.
- Gameplay rules remain in `UOceanInventoryComponent`, `UOceanSurvivalComponent`, `UOceanBuildGridComponent`, and `UOceanBuildComponent`.
- Drag-to-place must query `FOceanPlacementQueryResult` first; UI must not directly spawn build actors.

## Scope Boundary

This round is the UI/WBP/interaction infrastructure foundation. It is not final art, not a fully animated drawer, not fishing/diving/island-event UI, and not final replacement item icons.
