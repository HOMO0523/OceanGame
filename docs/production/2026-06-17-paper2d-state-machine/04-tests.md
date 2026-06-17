---
unit_id: 2026-06-17-paper2d-state-machine
status: approved
owner: test-designer
updated_at: 2026-06-17T16:55:00+08:00
source_commit: 2341511
depends_on: [2026-06-17-paper2d-animation-set]
parallel_lock: Ocean.Paper2DStateMachine
---

# Tests

| ID | Test | Method | Expected |
|---|---|---|---|
| T-001 | Direction mapping | `Ocean.Paper2D.Animation.Direction` | +X=`East`, -X=`West`, +Y=`North`, -Y=`South`, zero keeps last |
| T-002 | State priority | `Ocean.Paper2D.Animation.StatePriority` | Jump takes priority over walk; walk takes priority over idle |
| T-003 | Camera facing | `Ocean.Paper2D.Animation.CameraFacing` | Paper2D visual yaw faces camera within 1 degree |
| T-004 | Component exists | Bridge verifier | `BP_OceanSurvivorCharacter` has exactly one `OceanPaper2DAnimationComponent` |
| T-005 | Flipbook references | Bridge verifier | 24 imported Flipbooks are assigned |
| T-006 | Runtime loop sanity | no-LiveCoding pipeline | cold compile, editor relaunch, PIE, no `[TDD]` failures |
| T-007 | Save gate | Bridge dirty package probe | no dirty content or map packages after setup saves |
