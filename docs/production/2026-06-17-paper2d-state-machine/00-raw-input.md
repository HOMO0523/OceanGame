---
unit_id: 2026-06-17-paper2d-state-machine
status: draft
owner: intake
updated_at: 2026-06-17T16:55:00+08:00
source_commit: 2341511
depends_on: [2026-06-17-paper2d-animation-set]
parallel_lock: Ocean.Paper2DStateMachine
---

# Raw Input

## User Request

> paper2d的角色需要一直面向摄像机，然后需要一个状态机去切换动画，详细分析一下，现在这个只有一个动作哦，paper2d有什么功能是做这个的呢

> ok

## Interpreted Scope

- Keep the Paper2D visual layer on `BP_OceanSurvivorCharacter`.
- Make the sprite/flipbook visual component face the active top-down camera continuously.
- Add a lightweight animation state machine that selects imported Paper2D Flipbooks.
- Do not introduce full PaperZD or a third-party animation system in this slice.
- Do not move gameplay authority into Paper2D.
