---
unit_id: 2026-06-17-paperzd-pie-visibility
status: implemented
owner: implementer
updated_at: 2026-06-17T14:22:00+08:00
source_commit: working-tree
depends_on: [2026-06-17-paper2d-state-machine]
parallel_lock: Ocean.Paper2DVisibility
---

# Implementation Log

- Enabled the `PaperZD` plugin in `Ocean.uproject`.
- Updated UnrealBridge `StartPIE` to leave `StartLocation` unset when map `PlayerStart` actors exist.
- Added `[TDD] UnrealBridge_StartPIE_PlayerStartPolicy` to guard the spawn policy.
- Removed forced camera-facing rotation from `UOceanPaper2DAnimationComponent`; sprite angle is now manual.
- Remapped Paper2D direction selection so W/S read as up/down and the current A/D asset rows display correct screen-left/screen-right animation.
- Updated `Ocean.Paper2D.Animation.Direction` expectations to document the project-specific row mapping.
