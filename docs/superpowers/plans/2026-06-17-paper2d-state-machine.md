# Paper2D State Machine Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a lightweight Ocean Paper2D presentation state machine that keeps the survivor sprite facing the active camera and switches among imported Flipbooks.

**Architecture:** Add one focused `UOceanPaper2DAnimationComponent` owned by `AOceanCharacter`. The component reads character movement state and camera location, then controls the existing `UPaperFlipbookComponent`; it never owns movement, collision, inventory, interaction, or build gameplay.

**Tech Stack:** UE5.7 C++, Paper2D `UPaperFlipbookComponent`, Unreal Automation Tests, UnrealBridge no-LiveCoding pipeline.

---

## File Structure

- Create `Source/Ocean/OceanPrototype/OceanPaper2DAnimationComponent.h`: enum definitions, editable Flipbook set, component API, and camera-facing/state-selection contract.
- Create `Source/Ocean/OceanPrototype/OceanPaper2DAnimationComponent.cpp`: state selection, direction calculation, camera-facing rotation, and Flipbook switching.
- Modify `Source/Ocean/OceanCharacter.h`: add `UOceanPaper2DAnimationComponent*` and getter.
- Modify `Source/Ocean/OceanCharacter.cpp`: create the animation component and call its `UpdatePresentation` from `Tick`.
- Modify `Source/Ocean/Tests/OceanPaper2DAnimationTests.cpp`: automation tests for Flipbook lookup, state priority, and camera-facing yaw.
- Modify `scripts/setup_mvp_survival_loop.py`: assign imported Flipbooks onto the component defaults for `BP_OceanSurvivorCharacter`.
- Modify `scripts/verify_mvp_survival_loop.py`: verify component presence, 24 Flipbook references, and default state availability.
- Modify memory-bank and production docs after verification.

## Task 1: Red Tests

**Files:**
- Create: `Source/Ocean/Tests/OceanPaper2DAnimationTests.cpp`
- Modify: `scripts/verify_mvp_survival_loop.py`

- [ ] **Step 1: Write failing C++ automation tests**

Create tests that expect:

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanPaper2DAnimationDirectionTest, "Ocean.Paper2D.Animation.Direction", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanPaper2DAnimationStatePriorityTest, "Ocean.Paper2D.Animation.StatePriority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOceanPaper2DAnimationCameraFacingTest, "Ocean.Paper2D.Animation.CameraFacing", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
```

Expected failure before implementation: `UOceanPaper2DAnimationComponent` is undefined.

- [ ] **Step 2: Add failing Bridge verifier checks**

Add checks that expect `BP_OceanSurvivorCharacter` to contain exactly one `OceanPaper2DAnimationComponent` and 24 assigned Flipbooks.

- [ ] **Step 3: Verify RED**

Run:

```powershell
python scripts/ue_tdd_pipeline.py --no-launch
```

Expected: compile failure on missing `UOceanPaper2DAnimationComponent` or verifier failure on missing component/defaults.

## Task 2: Component Implementation

**Files:**
- Create: `Source/Ocean/OceanPrototype/OceanPaper2DAnimationComponent.h`
- Create: `Source/Ocean/OceanPrototype/OceanPaper2DAnimationComponent.cpp`
- Modify: `Source/Ocean/OceanCharacter.h`
- Modify: `Source/Ocean/OceanCharacter.cpp`

- [ ] **Step 1: Add enums and component API**

Define `EOceanPaper2DAnimationState` with `Idle`, `Walk`, `Jump`, `Swim`, `Climb`, `DiveSuitDive`, and `EOceanPaper2DDirection` with `South`, `West`, `East`, `North`.

- [ ] **Step 2: Add editable Flipbook set**

Expose 24 `UPaperFlipbook*` properties grouped by state/direction so Blueprint defaults can be assigned without custom editor tooling.

- [ ] **Step 3: Add state selection**

Implement priority:

```text
DiveSuitDive > Climb > Jump > Swim > Walk > Idle
```

For this MVP, only `Jump`, `Walk`, and `Idle` are auto-derived; `Swim`, `Climb`, and `DiveSuitDive` remain callable/assignable states for future event systems.

- [ ] **Step 4: Add camera-facing update**

Rotate only `Paper2DVisualComponent` yaw toward the active camera. Do not rotate the character actor or movement component.

- [ ] **Step 5: Wire component into `AOceanCharacter`**

Create `OceanPaper2DAnimationComponent` in the constructor and call `UpdatePresentation(DeltaSeconds)` in `Tick`.

## Task 3: Asset Binding

**Files:**
- Modify: `scripts/setup_mvp_survival_loop.py`
- Modify: `scripts/verify_mvp_survival_loop.py`
- Modify UE asset: `Content/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter.uasset`

- [ ] **Step 1: Bind imported Flipbooks**

Map imported assets from `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe/Flipbooks` to the 24 component properties.

- [ ] **Step 2: Verify asset defaults**

Bridge verifier prints:

```text
[TDD] MVPPaper2DAnimComponent: actual=1 expected=1 result=PASS
[TDD] MVPPaper2DAnimFlipbooks: actual=24 expected=24 result=PASS
```

## Task 4: Green Verification

**Files:**
- No new implementation files; verification only.

- [ ] **Step 1: Run cold compile and PIE**

```powershell
python scripts/ue_tdd_pipeline.py --pie-duration 3 --log-lines 160
```

Expected: compile succeeds, editor relaunches, PIE runs, no `[TDD]` failures.

- [ ] **Step 2: Run C++ automation**

```powershell
D:\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe D:\UE5 demo\Ocean\Ocean.uproject -unattended -nop4 -nosplash -NullRHI -ExecCmds="Automation RunTests Ocean.Paper2D; Quit" -TestExit="Automation Test Queue Empty"
```

Expected: all `Ocean.Paper2D.Animation.*` tests pass.

- [ ] **Step 3: Run MVP verifier**

Use `BridgeClient` to execute `scripts/setup_mvp_survival_loop.py`, then `scripts/verify_mvp_survival_loop.py`.

Expected: input, starter map, Paper2D component, and 24 Flipbook assignments pass.

## Task 5: Docs, Commit, Push

**Files:**
- Modify: `memory-bank/architecture.md`
- Modify: `memory-bank/progress.md`
- Modify: `memory-bank/tech-stack.md`
- Modify: `docs/defense/index.html`
- Modify: `docs/production/2026-06-17-paper2d-state-machine/05-implementation-log.md`
- Modify: `docs/production/2026-06-17-paper2d-state-machine/06-test-results.md`
- Modify: `docs/production/2026-06-17-paper2d-state-machine/07-review.md`

- [ ] **Step 1: Update docs with implemented boundaries**

State that Paper2D owns only camera-facing and Flipbook switching.

- [ ] **Step 2: Run pre-commit gates**

```powershell
python scripts/harness_state_validator.py --json
python scripts/doc_sync_hook.py --phase pre-commit --apply-memory --history --quiet
git diff --check
```

- [ ] **Step 3: Commit and push**

```powershell
git add <changed-files>
git commit -m "feat: add Paper2D animation state machine"
git push origin codex/ocean-phase-one-workflows
```

## Self-Review

- Spec coverage: camera-facing, state switching, imported Flipbooks, and gameplay-boundary protection are covered.
- Placeholder scan: no TBD/TODO placeholders.
- Type consistency: component, enum, and method names are stable across tasks.
