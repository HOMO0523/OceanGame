---
unit_id: 2026-06-17-hud-backpack-drawer-uiux
status: approved
owner: infra-auditor
updated_at: 2026-06-17T16:10:00+08:00
source_commit: 76ddd07
depends_on: [2026-06-16-minimal-loop-workflow]
parallel_lock: Ocean.UI.InventoryPlacement
---

# Infrastructure Audit

decision: needs-infra

| Requirement | Status | Evidence | Needed Change |
|---|---|---|---|
| UMG dependency | supported | `Source/Ocean/Ocean.Build.cs` includes `UMG` and `Slate`. | No dependency change expected. |
| Survival stats | supported | `UOceanSurvivalComponent` exposes stamina, hydration, and satiety getters. | Add explicit recovery API or item-use helper. |
| Inventory capacity | partial | `UOceanInventoryComponent` has stack capacity through `MaxSlots`. | Add slot item model while preserving resource-stack compatibility. |
| Item definitions | missing | `EOceanResourceType` only covers `Wood`, `Scrap`, `Food`, `Water`. | Add item category/effect/slot view structures or data assets. |
| Use item | missing | No item-use command exists. | Add atomic use-item path with survival recovery and inventory decrement. |
| Drag/drop model | missing | No slot movement or drag result model exists. | Add merge/swap/invalid drag functions and tests. |
| Build placement | partial | `TryPlaceSelectedModuleAtWorld` places immediately and returns a generic message. | Add placement query result with failure reason before actual placement. |
| Grid zones | partial | Grid tracks occupancy and adjacency only. | Add query failure reasons first; zone expansion can follow after base UI works. |
| Backpack key | missing | `B` is already build toggle. | Add `IA_OceanToggleBackpack` with `Tab/I` and bind to UI state. |
| Ocean WBP assets | missing | Content scan found template UI but no Ocean backpack WBP. | Create `/Game/OceanPrototype/UI/*` assets through UnrealBridge script. |
| PIE UI verification | missing | Existing tests cover build/grid/Paper2D, not WBP. | Add bridge verifier script for asset existence and HUD spawn. |
| Defense documentation | partial | Defense page mentions UI work generally. | Update after implementation with screenshot/probe evidence. |
