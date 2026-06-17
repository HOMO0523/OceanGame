---
unit_id: 2026-06-17-paper2d-state-machine
status: approved
owner: planner
updated_at: 2026-06-17T16:55:00+08:00
source_commit: 2341511
depends_on: [2026-06-17-paper2d-animation-set]
parallel_lock: Ocean.Paper2DStateMachine
---

# Plan

## Files Touched

- `Source/Ocean/OceanPrototype/OceanPaper2DAnimationComponent.h`
- `Source/Ocean/OceanPrototype/OceanPaper2DAnimationComponent.cpp`
- `Source/Ocean/OceanCharacter.h`
- `Source/Ocean/OceanCharacter.cpp`
- `Source/Ocean/Tests/OceanPaper2DAnimationTests.cpp`
- `scripts/setup_mvp_survival_loop.py`
- `scripts/verify_mvp_survival_loop.py`
- `Content/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter.uasset`

## TDD Logs / Tests First

- Add `Ocean.Paper2D.Animation.Direction`.
- Add `Ocean.Paper2D.Animation.StatePriority`.
- Add `Ocean.Paper2D.Animation.CameraFacing`.
- Add verifier probes:
  - `[TDD] MVPPaper2DAnimComponent`
  - `[TDD] MVPPaper2DAnimFlipbooks`

## Implementation Order

1. Write red C++ automation tests and verifier expectations.
2. Confirm red through no-LiveCoding build or verifier failure.
3. Add `UOceanPaper2DAnimationComponent`.
4. Wire component into `AOceanCharacter`.
5. Extend setup script to assign 24 imported Flipbooks to Blueprint defaults.
6. Run setup and verifier through UnrealBridge.
7. Run cold compile, PIE, and command-line automation tests.
8. Update docs and commit.

## Rollback / Deviation Rule

- If imported Flipbook assets are missing, stop at infra-audit and do not create placeholder Flipbooks in code.
- If Paper2D plugin APIs differ, update `02-infra-audit.md` before changing architecture.

## Done Criteria

- 24 Flipbook references are assigned on `BP_OceanSurvivorCharacter`.
- Component switches `Idle`, `Walk`, and `Jump` automatically.
- Component exposes future visual states for `Swim`, `Climb`, and `DiveSuitDive`.
- Paper2D visual faces the camera without rotating the character actor.
- C++ tests, MVP verifier, harness validator, and diff check pass.

## Non-Goals

- No PaperZD plugin.
- No full underwater gameplay.
- No final art quality acceptance for side-walk leg continuity.
- No replacement of `BP_OceanSurvivorCharacter`.
