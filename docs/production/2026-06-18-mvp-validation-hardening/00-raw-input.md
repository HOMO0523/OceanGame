---
unit_id: 2026-06-18-mvp-validation-hardening
status: active
owner: intake
updated_at: 2026-06-18T14:05:00+08:00
source_commit: 486a9d5
depends_on: [2026-06-16-mvp-survival-loop, 2026-06-17-hud-backpack-drawer-uiux]
parallel_lock: Ocean.MVPValidation
---

# Raw Input

User request:

> 先制作并执行完整测试，然后记录卡点和问题，过程中遇到卡点修复，保证mvp的正式落地

Follow-up:

> git先传一版本到远程仓库里
> 继续

Checkpoint pushed before this unit:

- Branch: `codex/ocean-phase-one-workflows`
- Commit: `486a9d5 chore: checkpoint before MVP validation fixes`
- Remote: `origin/codex/ocean-phase-one-workflows`

Known incoming failures from review:

- `Automation RunTests Ocean` fails at `Ocean.Resources.Node.HasPlaceholderAndBuoyancy`.
- `Automation RunTests Ocean` then crashes at `Ocean.UI.BackpackPanel.RefreshSlots`.
- `scripts/verify_mvp_survival_loop.py` is stale against current input/GameMode/PlayerStart state.
- `scripts/verify_ocean_ui_assets.py --pie` fails from the main menu because it expects the game-map HUD log.
