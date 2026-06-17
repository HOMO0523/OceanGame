---
unit_id: 2026-06-17-hud-backpack-drawer-uiux
status: approved
owner: planner
updated_at: 2026-06-17T16:10:00+08:00
source_commit: 76ddd07
depends_on: [2026-06-16-minimal-loop-workflow]
parallel_lock: Ocean.UI.InventoryPlacement
---

# Plan

## Files Touched

- `Source/Ocean/OceanPrototype/OceanItemTypes.h`
- `Source/Ocean/OceanPrototype/OceanInventoryComponent.h`
- `Source/Ocean/OceanPrototype/OceanInventoryComponent.cpp`
- `Source/Ocean/OceanPrototype/OceanSurvivalComponent.h`
- `Source/Ocean/OceanPrototype/OceanSurvivalComponent.cpp`
- `Source/Ocean/OceanPrototype/OceanBuildPlacementTypes.h`
- `Source/Ocean/OceanPrototype/OceanBuildGridComponent.h`
- `Source/Ocean/OceanPrototype/OceanBuildGridComponent.cpp`
- `Source/Ocean/OceanPrototype/OceanBuildComponent.h`
- `Source/Ocean/OceanPrototype/OceanBuildComponent.cpp`
- `Source/Ocean/OceanPrototype/UI/OceanHUDRootWidget.h`
- `Source/Ocean/OceanPrototype/UI/OceanHUDRootWidget.cpp`
- `Source/Ocean/OceanPlayerController.h`
- `Source/Ocean/OceanPlayerController.cpp`
- `Source/Ocean/Tests/OceanMVPInventoryTests.cpp`
- `Source/Ocean/Tests/OceanMVPPlacementQueryTests.cpp`
- `Source/Ocean/Tests/OceanMVPUIModelTests.cpp`
- `scripts/create_ocean_ui_assets.py`
- `scripts/verify_ocean_ui_assets.py`
- `docs/defense/index.html`
- `memory-bank/architecture.md`
- `memory-bank/progress.md`
- `memory-bank/tech-stack.md`

## `[TDD]` Logs To Add Before C++ Implementation

- `[TDD] OceanInventoryUseItem: item=<id> result=<PASS|FAIL> stamina=<value> hydration=<value> satiety=<value>`
- `[TDD] OceanInventoryDragDrop: from=<index> to=<index> result=<Merge|Swap|Reject>`
- `[TDD] OceanPlacementQuery: can_place=<0|1> reason=<reason> anchor=<x,y>`
- `[TDD] OceanUIToggleBackpack: open=<0|1> input=<Tab|I|Button>`
- `[TDD] OceanUIAssetsExist: asset=<path> result=<PASS|FAIL>`
- `[TDD] OceanHUDRootPIE: created=<0|1> drawer_open=<0|1>`

## Implementation Order

1. Add inventory item model and tests.
2. Add survival recovery API and item-use tests.
3. Add inventory drag/drop model and tests.
4. Add build placement query and tests.
5. Add HUD root C++ state model and tests.
6. Add backpack input action and controller binding.
7. Add UnrealBridge script to create WBP assets.
8. Add UnrealBridge script to verify WBP assets and PIE spawn.
9. Update docs and defense summary.
10. Run no-LiveCoding pipeline and commit.

## Rollback / Deviation Rule

If C++ tests pass but PIE does not show the widget, return to asset creation and controller wiring before changing gameplay semantics.

If placement preview cannot be fully implemented in one pass, keep the query API and WBP overlay but do not fake successful placement from UI.

## Done Criteria

- All new automation tests pass.
- `WBP_OceanHUDRoot` and child widgets exist under `/Game/OceanPrototype/UI`.
- PIE on `L_WaterOcean` creates the HUD root.
- `Tab/I` opens and closes the backpack drawer without changing `B` build behavior.
- Inventory use and drag/drop tests cannot lose items.
- Placement query returns typed failure reasons before placement.
- Docs and defense HTML mention the right-side drawer and UI/gameplay boundary.

## Non-Goals

- Final icon art.
- Animated drawer polish.
- Full fishing, diving, island, or cruise UI.
- Complete grid zone authoring beyond the base query failure reasons.

## Detailed Plan File

The copyable task plan is saved at:

- `docs/superpowers/plans/2026-06-17-ocean-hud-backpack-drawer-uiux.md`
