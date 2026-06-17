---
unit_id: 2026-06-17-paper2d-state-machine
status: approved
owner: infra-auditor
updated_at: 2026-06-17T16:55:00+08:00
source_commit: 2341511
depends_on: [2026-06-17-paper2d-animation-set]
parallel_lock: Ocean.Paper2DStateMachine
---

# Infrastructure Audit

| Requirement | Status | Evidence | Needed Change |
|---|---|---|---|
| Paper2D plugin | supported | `Ocean.uproject`, `Source/Ocean/Ocean.Build.cs` include Paper2D | None |
| Imported Flipbooks | supported | `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe/Flipbooks` has 24 Flipbooks | Bind them to component defaults |
| Visual component | partial | `AOceanCharacter::Paper2DVisualComponent` exists | Add animation component that controls it |
| Camera-facing billboard | missing | Current component has static relative rotation | Add camera-facing update |
| Animation state selection | missing | Default `idle_south` only | Add lightweight state machine |
| Gameplay boundary | supported | `BP_OceanSurvivorCharacter` remains Ocean pawn | Keep movement/collision outside Paper2D |
| Automation tests | partial | Existing `Ocean.Build` tests and MVP verifier | Add `Ocean.Paper2D.Animation` tests and verifier checks |

```yaml
decision: needs-infra
```
