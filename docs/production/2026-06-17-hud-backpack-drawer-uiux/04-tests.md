---
unit_id: 2026-06-17-hud-backpack-drawer-uiux
status: approved
owner: test-designer
updated_at: 2026-06-17T16:10:00+08:00
source_commit: 76ddd07
depends_on: [2026-06-16-minimal-loop-workflow]
parallel_lock: Ocean.UI.InventoryPlacement
---

# Tests

| ID | Test | Method | Expected |
|---|---|---|---|
| T-001 | Use food item | C++ automation `Ocean.MVP.Inventory.UseItem` | Food slot decrements by one and satiety clamps to `100`. |
| T-002 | Use water item | C++ automation `Ocean.MVP.Inventory.UseItem` | Water slot decrements by one and hydration clamps to `100`. |
| T-003 | Use missing item | C++ automation `Ocean.MVP.Inventory.UseItem` | Returns failure, no stats change, no inventory mutation. |
| T-004 | Drag merge | C++ automation `Ocean.MVP.Inventory.DragDropModel` | Same item stacks merge up to max stack without losing remainder. |
| T-005 | Drag swap | C++ automation `Ocean.MVP.Inventory.DragDropModel` | Different item slots swap exactly. |
| T-006 | Invalid drag | C++ automation `Ocean.MVP.Inventory.DragDropModel` | Invalid index returns reject and all slots remain unchanged. |
| T-007 | Placement occupied | C++ automation `Ocean.MVP.Build.PlacementQuery` | Occupied core cell returns `OccupiedCell`. |
| T-008 | Placement detached | C++ automation `Ocean.MVP.Build.PlacementQuery` | Detached cell returns `DetachedFromPlatform`. |
| T-009 | Placement affordable adjacent | C++ automation `Ocean.MVP.Build.PlacementQuery` | Adjacent valid deck returns `bCanPlace=true`. |
| T-010 | HUD state model | C++ automation `Ocean.MVP.UI.HUDStateModel` | Toggle backpack changes state and records `GameAndUI` input intent. |
| T-011 | WBP asset existence | UnrealBridge `scripts/verify_ocean_ui_assets.py` | All accepted asset paths load successfully. |
| T-012 | PIE HUD spawn | UnrealBridge `scripts/verify_ocean_ui_assets.py --pie` | `WBP_OceanHUDRoot` is present and drawer toggles open/closed. |
| T-013 | Build key regression | PIE log probe | `B` still emits `[TDD] OceanBuildToggleHandled: result=PASS`. |
| T-014 | Backpack key | PIE log probe | `Tab` or `I` emits `[TDD] OceanUIToggleBackpack: open=1`. |
