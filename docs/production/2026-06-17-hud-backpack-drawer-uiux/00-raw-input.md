---
unit_id: 2026-06-17-hud-backpack-drawer-uiux
status: approved
owner: intake
updated_at: 2026-06-17T16:10:00+08:00
source_commit: 76ddd07
depends_on: [2026-06-16-minimal-loop-workflow]
parallel_lock: Ocean.UI.InventoryPlacement
---

# Raw Input

## User Provided Intent

The user approved the UI/UX direction derived from the planner sketches:

- The closed HUD keeps the screen mostly clear.
- The open inventory is a right-side backpack drawer.
- The HUD must support the small-boat survival loop rather than a full-screen RPG inventory.
- The first implementation should create solid UI/WBP infrastructure before final art replacement.

## Visual References

- `C:/Users/shxuw/xwechat_files/wxid_g90er9r4o8p312_cd3c/temp/RWTemp/2026-06/93b72d0770041befdeb6f18e6e3229c9/8a3d849a88e9c9035d0b86d179cbd00c.png`
- `C:/Users/shxuw/xwechat_files/wxid_g90er9r4o8p312_cd3c/temp/RWTemp/2026-06/ad284d070cc884a6564f805dabd76de0.png`

The sketch shows:

- Left-top survival status.
- Top-center day/time.
- Right-top backpack and direction indicators.
- Center boat/play area.
- Left-bottom menu entry.
- Right-side backpack drawer when opened.

## Interpreted Scope

This production unit covers the first implementable UI foundation for:

- `WBP_OceanHUDRoot`
- `WBP_StatusPanel`
- `WBP_TimePanel`
- `WBP_TopRightPanel`
- `WBP_BackpackDrawer`
- `WBP_InventorySlot`
- `WBP_ItemDragVisual`
- `WBP_PlacementOverlay`
- `WBP_ConfirmModal`
- `WBP_ToastStack`

The implementation must connect to existing Ocean survival, inventory, build, and input systems without replacing their gameplay authority.
