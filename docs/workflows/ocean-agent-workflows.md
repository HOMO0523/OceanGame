# Ocean Agent Workflows

This document adapts the reusable workflow patterns from the reference Unreal project into the Ocean prototype. It is intentionally project-local: use `{ProjectRoot}` for this repository and `{UE_ROOT}` for the installed Unreal Engine root.

## Operating Rules

- Keep feature work off `main`. Use a topic branch such as `codex/ocean-phase-one-workflows`.
- Prefer project-owned files under `Content/OceanPrototype` and `Source/Ocean/OceanPrototype` for Ocean-specific work.
- Do not overwrite hand-authored Blueprint or map layout work. If an asset exists, inspect it before replacing it.
- If a visual asset is missing, use a simple cube placeholder first. The placeholder actor must keep the same gameplay components so the mesh can be replaced later without changing code.
- Actor assets that represent floating resources, floating modules, or waterborne props must include a buoyancy path. In C++ this means `UBuoyancyComponent`; in Blueprint this means a derived actor that keeps the inherited buoyancy component.
- Keep the logical construction grid stable even if visuals bob on the water.

## Unreal Editor Automation

For UE editor actions, prefer automation before asking for manual editor clicks:

1. If the editor is running and an UnrealBridge-style endpoint is available, use it to execute Python, save dirty packages, run PIE, and inspect logs.
2. If no bridge is available yet, add project scripts that make the same steps repeatable instead of relying on ad-hoc manual actions.
3. Before closing or killing the editor, save dirty packages and verify key assets can be loaded.
4. If the editor may contain unsaved user edits and no automation connection is available, stop and report the blocker instead of killing the editor.

## Compile Workflow

Use a cold no-LiveCoding compile for C++ verification:

```powershell
& "{UE_ROOT}\Engine\Build\BatchFiles\Build.bat" OceanEditor Win64 Development "{ProjectRoot}\Ocean.uproject" -NoHotReload
```

Do not use Live Coding or Hot Reload as completion evidence for C++ changes. If the editor is open, save dirty packages first, close the editor, then compile.

## TDD Workflow

Use this loop for gameplay systems:

1. **Plan**: Define the behavior and expected evidence.
2. **Instrument**: Add an automated test or `[TDD]` log check before trusting implementation.
3. **Implement**: Write the smallest code that makes the test pass.
4. **Build & Run**: Run the cold compile path or an editor automation script.
5. **Analyze**: Compare test output, `[TDD]` logs, or automation reports against expected values.

For Ocean phase one, preferred early tests are C++ automation tests for grid math, footprint rotation, overlap checks, and resource-cost validation. PIE or bridge tests should cover map, Water, PCG, and component presence once editor asset generation is available.

For menu-to-game routing, run:

```powershell
python scripts/verify_main_menu_navigation.py
```

This probe starts PIE on `L_MainMenu`, confirms the runtime `NewGameButton` delegate is bound, broadcasts the click, and verifies the active PIE world travels to `L_WaterOcean`.

For menu/settings/pause regression checks, run:

```powershell
& 'D:\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' 'D:\UE5 demo\Ocean\Ocean.uproject' -unattended -nop4 -nosplash -NullRHI -NoSound -NoLiveCoding -ExecCmds='Automation RunTests Ocean.UI; Quit' -TestExit='Automation Test Queue Empty'
```

Expected evidence includes 18 completed `Ocean.UI` tests plus `[TDD] OceanSettingsBindings: bgm=1 sfx=1 save=1 close=1 centered=1`, `[TDD] OceanPauseMenuBindings: resume=1 settings=1 quit=1 save_list=1`, and `[TDD] OceanSaveSlotBindings: slot=1 load=1 delete=1`.

## GPT / DS Handoff Workflow

Use this workflow when the user asks GPT to clarify scope and stop before code:

1. **GPT context pass**: read `AGENTS.md`, memory-bank docs, active production units, relevant specs/plans, and any user design documents.
2. **GPT spec freeze**: write the user-visible semantics, edge cases, deferred scope, and acceptance criteria under `docs/superpowers/specs` or the active `docs/production` unit.
3. **GPT TDD strategy**: list the automation tests, `[TDD]` probes, bridge checks, and manual review gates required for the eventual code work.
4. **Stop before code**: do not modify C++/Blueprint/map assets until the user confirms the scope and chooses the DS/code executor.
5. **DS execution**: implement from the approved plan, test-first, one subsystem at a time.
6. **Automation gate**: save dirty editor packages, close the editor, cold compile, relaunch, run PIE/automation, capture logs, then sync docs.
7. **Git record**: run doc sync, `git diff --check`, commit a scoped summary, and push the branch.

For the current minimal loop, GPT owns the layout/spec/TDD documents and DS should start only after the user confirms the open questions in `docs/superpowers/specs/2026-06-16-ocean-minimal-loop-spec-and-workflow.md`.

## Production Unit Files

For larger features, create a production unit under `docs/production/{date}-{unit}` with:

- `00-raw-input.md`: original request and assumptions.
- `01-semantics.md`: frozen user-visible behavior.
- `02-infra-audit.md`: existing support, missing support, forbidden shortcuts.
- `03-plan.md`: exact files, test evidence, and implementation order.
- `04-tests.md`: test names, commands, and expected output.
- `05-implementation-log.md`: concise execution notes.
- `06-test-results.md`: actual command output or blocker.
- `07-review.md`: review findings and follow-ups.

Use `parallel_lock` metadata when multiple agents or threads might touch related systems. Examples for Ocean:

- `Ocean.BuildGrid`
- `Ocean.WaterSetup`
- `Ocean.ResourcePCG`
- `Ocean.BuildModeInput`
- `Ocean.PlaceholderAssets`

## Documentation Update Rules

| Event | Update Target |
|---|---|
| New reusable workflow or hard constraint | `docs/workflows/ocean-agent-workflows.md` |
| Feature design changes | `docs/superpowers/specs/*` |
| Implementation sequence changes | `docs/superpowers/plans/*` |
| Test or automation command changes | `docs/workflows/ocean-agent-workflows.md` or the active production unit |
| Project architecture decisions | `docs/workflows/ocean-architecture-notes.md` when it exists |

## Useful Skills

| Skill | When To Use |
|---|---|
| `brainstorming` | Before changing gameplay behavior or design direction. |
| `writing-plans` | Before implementing a multi-step feature from an approved spec. |
| `test-driven-development` | Before writing feature or bugfix code. |
| `using-git-worktrees` | Before executing a plan that should be isolated from the current checkout. |
| `executing-plans` | When implementing a written plan task-by-task in the current session. |
| `verification-before-completion` | Before claiming a task is complete, pushed, fixed, or passing. |
| `unreal-bridge` | When controlling a running UE editor, creating assets, saving packages, running PIE, or reading runtime state. |
| `github:yeet` | When publishing local commits or opening a PR. |

## Migrated Automation Chain

The following reference-project automation pieces have been migrated and Ocean-adapted:

- `scripts/ue_tdd_bridge.py`: bridge client for endpoint discovery, PIE control, and log capture.
- `scripts/ue_tdd_pipeline.py`: full save, cold compile, launch, PIE, capture, analyze loop for `OceanEditor`.
- `scripts/verify_main_menu_navigation.py`: real PIE main-menu New Game binding and level-travel verifier.
- `scripts/harness_state_validator.py`: validates production unit state files and parallel locks.
- `scripts/doc_sync_hook.py`: explicit pre-commit or manual documentation drift detector.
- `Plugins/UnrealBridge`: editor-only TCP/Python bridge source plugin, copied without generated binaries.
- `.agents/skills/ue-tdd-livecoding`: no-LiveCoding UE TDD skill, adapted to Ocean.
- `.agents/skills/ocean-ai-production-hardness`: Ocean-specific production loop skill.

The Roguelike gameplay scripts and plugins were intentionally not copied because their state, UI, economy, and balance assumptions do not belong to Ocean.
