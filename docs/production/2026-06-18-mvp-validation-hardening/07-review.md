---
unit_id: 2026-06-18-mvp-validation-hardening
status: active
owner: reviewer
updated_at: 2026-06-18T14:05:00+08:00
source_commit: 486a9d5
depends_on: [2026-06-16-mvp-survival-loop, 2026-06-17-hud-backpack-drawer-uiux]
parallel_lock: Ocean.MVPValidation
---

# Review

## Defects And Boundaries

| Priority | Finding | Impact | Disposition |
|---|---|---|---|
| P1 | Bare `Automation RunTests Ocean` is a false aggregation command. | It runs unrelated UE Water plugin tests with “Ocean” in their names and can fail even when project tests are green. | Use grouped project prefixes: `Ocean.Build`, `Ocean.Resources`, `Ocean.UI`, `Ocean.MVP`, `Ocean.Paper2D`. |
| P1 | Unreal Python cannot sleep inside a Bridge execution and still expect PIE ticks. | Self-contained in-editor `--pie` probes can false-fail because the editor tick is blocked. | Added external `scripts/verify_ocean_ui_pie.py` to load map, start PIE, wait client-side, and verify HUD log. |
| P2 | UI tests that call `TakeWidget()` on raw `NewObject<UUserWidget>` need `Initialize()`. | Without it, native `RebuildWidget()` sees `WidgetTree == null` and can fatal in commandlets. | Updated tests; future UI tests should use the same lifecycle or `CreateWidget`. |
| P2 | Existing UI font constructors are deprecated in UE5.7. | Not blocking MVP, but future engine upgrades may break compile. | Track as follow-up cleanup, not part of this validation unit. |
| P2 | `OceanDayNightCycleComponent` still has C4701 warnings for `Rot` and `Color`. | Non-blocking warning, but it can hide real lighting bugs later. | Record as follow-up hardening. |
| P3 | Untracked `scripts/tmp_*` and `ocean.docx` remain in the workspace. | Risk of staging temporary debugging files. | Do not stage unless explicitly promoted; inspect `git status --short` before commit. |

## Acceptance State

- Project C++ automation, MVP asset verification, UI asset verification, and gameplay-map HUD PIE probe are green as of 2026-06-18 14:36.
- Remaining work before push: final static/doc-sync checks, progress memory update, commit, and remote push.
