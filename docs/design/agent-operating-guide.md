# Ocean Agent Operating Guide

This guide is the low-context handoff entry point for Ocean agents. It lets a new agent resume from files without relying on chat history.

## Authority Chain

Read and obey documents in this order:

1. `AGENTS.md`: project law.
2. `memory-bank/architecture.md`, `memory-bank/progress.md`, `memory-bank/tech-stack.md`: current project facts.
3. `docs/workflows/ocean-agent-workflows.md`: reusable operating patterns.
4. Active `docs/production/{date}-{unit}/00..07-*.md`: production-unit contract for the current task.
5. Current `docs/superpowers/specs/*`: feature semantics and approved designs.
6. `docs/superpowers/plans/*`: implementation history and task breakdown.
7. `.agents/skills/*/SKILL.md`: local reusable workflows.

If documents conflict, update the higher-authority file first.

## First 10 Minutes Checklist

For any non-trivial task:

1. Read `AGENTS.md`.
2. Read this guide.
3. Read the three `memory-bank` files.
4. Search active `docs/production/` units.
5. Search relevant specs/plans with `rg`.
6. State which skill or workflow applies.
7. If C++ changes are needed, define `[TDD]` probes or automation tests before implementation.
8. Preserve user-edited maps, Blueprints, PCG graphs, materials, and camera work.
9. After verification, record progress and save status.

## Skill Trigger Matrix

| Task | Use Skill Or Workflow | Required Behavior |
|---|---|---|
| UE C++ gameplay/tooling change | `.agents/skills/ue-tdd-livecoding/SKILL.md` | Plan `[TDD]` logs or automation tests first, then run no-LiveCoding pipeline. |
| Ocean feature slice | `.agents/skills/ocean-ai-production-hardness/SKILL.md` | Freeze semantics, audit Water/PCG/build-grid support, verify user-visible playability. |
| Running editor automation | `unreal-bridge` plus `scripts/ue_tdd_bridge.py` | Save packages, create assets, run PIE, inspect logs through bridge. |
| Multi-step implementation plan | `superpowers:writing-plans` | Save a plan under `docs/superpowers/plans/`. |
| Bug investigation | `superpowers:systematic-debugging` | Reproduce, isolate, test hypothesis, patch, rerun. |
| Completion, commit, push, PR | `superpowers:verification-before-completion` plus git workflow | Run fresh verification before claiming success or pushing. |

If no Skill tool exists in the current agent environment, open the local `SKILL.md` file and follow it.

## UE Editor And C++ Workflow

Never use Live Coding or Hot Reload as proof.

1. Plan expected `[TDD]` lines or automation-test assertions.
2. Add probes/tests before behavior changes.
3. Implement the minimal behavior.
4. Run:

```powershell
python scripts/ue_tdd_pipeline.py --pie-duration 5
```

The pipeline saves dirty packages through UnrealBridge, closes the editor, cold compiles `OceanEditor`, launches the editor, runs PIE, captures logs, and analyzes `[TDD]` lines.

Use `--check-only` only to inspect logs in a running editor. It is not compile verification.

If the editor may have unsaved changes and bridge cannot connect, do not kill it. Report the save gate blocker.

## Ocean Prototype Acceptance

For phase-one work, verify the following before calling a slice complete:

| Area | Acceptance |
|---|---|
| Water | `Water` plugin enabled, `WaterBodyCollision` profile present, ocean map/actor can load. |
| Build grid | Modules reserve multi-cell footprints; overlaps are rejected; rotation behavior is deterministic. |
| Placeholder assets | Missing meshes use cube placeholders without losing gameplay components. |
| Buoyancy | Floating modules and resource nodes keep `UBuoyancyComponent` or inherited equivalent. |
| PCG resources | PCG component/graph path exists, or deterministic fallback spawn is documented and tested. |
| Save gate | Generated maps/assets are saved and re-loadable before editor restart or handoff. |

## Production Unit Workflow

Use `docs/production/{YYYY-MM-DD}-{unit-id}/` for larger tasks. Each unit has:

- `00-raw-input.md`
- `01-semantics.md`
- `02-infra-audit.md`
- `03-plan.md`
- `04-tests.md`
- `05-implementation-log.md`
- `06-test-results.md`
- `07-review.md`

Use `parallel_lock` values such as:

- `Ocean.BuildGrid`
- `Ocean.WaterSetup`
- `Ocean.ResourcePCG`
- `Ocean.BuildModeInput`
- `Ocean.PlaceholderAssets`
- `Ocean.EditorAutomation`

## Useful Commands

```powershell
python scripts/harness_state_validator.py --json
python scripts/doc_sync_hook.py --phase manual
python scripts/ue_tdd_pipeline.py --check-only
& "{UE_ROOT}\Engine\Build\BatchFiles\Build.bat" Ocean Win64 Development "{ProjectRoot}\Ocean.uproject" -NoHotReload
& "{UE_ROOT}\Engine\Build\BatchFiles\Build.bat" OceanEditor Win64 Development "{ProjectRoot}\Ocean.uproject" -NoHotReload
git diff --check
```

## Git Upload Workflow

Before pushing:

1. Run verification appropriate to changed files.
2. Run the doc-sync hook if the user requested commit/staging/push.
3. Run `git diff --check`.
4. Inspect `git status --short`.
5. Stage intended files only.
6. Commit and push the active branch.
