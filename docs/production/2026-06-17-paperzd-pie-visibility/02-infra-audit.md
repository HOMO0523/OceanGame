---
unit_id: 2026-06-17-paperzd-pie-visibility
status: approved
owner: infra-auditor
updated_at: 2026-06-17T14:22:00+08:00
source_commit: working-tree
depends_on: [2026-06-17-paper2d-state-machine]
parallel_lock: Ocean.Paper2DVisibility
---

# Infrastructure Audit

| Requirement | Status | Evidence | Needed Change |
|---|---|---|---|
| PaperZD plugin | supported | `Ocean.uproject` enables `PaperZD` | Keep plugin enabled; do not assume it drives the pawn yet |
| Existing Paper2D visual | supported | `BP_OceanSurvivorCharacter` owns `Paper2DVisual` and `OceanPaper2DAnimation` | Preserve existing visual pipeline |
| PIE spawn policy | broken before fix | Bridge `StartPIE` forced editor viewport `StartLocation` | Use map `PlayerStart` when present |
| Manual sprite angle | user-owned | User chose to tune Paper2D angle manually | Remove forced camera-facing rotation |
| Direction rows | needs project mapping | W/S correct after remap; A/D rows are visually swapped | Encode mapping in tests |

```yaml
decision: needs-fix
```
