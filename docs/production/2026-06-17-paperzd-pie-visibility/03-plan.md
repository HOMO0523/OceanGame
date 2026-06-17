---
unit_id: 2026-06-17-paperzd-pie-visibility
status: approved
owner: planner
updated_at: 2026-06-17T14:22:00+08:00
source_commit: working-tree
depends_on: [2026-06-17-paper2d-state-machine]
parallel_lock: Ocean.Paper2DVisibility
---

# Plan

1. Confirm PaperZD loads without replacing the existing Ocean Paper2D component.
2. Add TDD evidence for UnrealBridge `StartPIE` PlayerStart policy.
3. Fix Bridge PIE spawn transform so maps with `APlayerStart` keep authored spawn points.
4. Remove automatic Paper2D camera-facing rotation after manual-angle decision.
5. Correct Paper2D direction mapping for WASD screen readability.
6. Run doc sync, diff check, and no-LiveCoding cold compile before git upload.

## Non-Goals

- No PaperZD animation blueprint wiring in this slice.
- No replacement of `BP_OceanSurvivorCharacter`.
- No final art-direction acceptance for Paper2D sprite angle.
