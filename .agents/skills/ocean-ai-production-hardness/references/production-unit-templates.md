# Production Unit Templates

Use these templates when applying `ocean-ai-production-hardness`.

## Metadata

Every `docs/production/{YYYY-MM-DD}-{unit-id}/00..07-*.md` file starts with:

```yaml
---
unit_id: YYYY-MM-DD-unit-id
status: draft|frozen|approved|updated|blocked|reviewed
owner: intake|clarifier|infra-auditor|planner|test-designer|implementer|test-runner|reviewer
updated_at: YYYY-MM-DDTHH:MM:SS
source_commit: working-tree|<sha>
depends_on: []
parallel_lock: Subsystem.Contract
---
```

## 00 Raw Input

Record the original user request, video/design references, screenshots, logs, and linked docs without rewriting intent. Add a short "interpreted scope" only after the raw material.

## 01 Semantics

Required fields for every unit:

```yaml
unit_type: infra|bugfix|command|water_setup|build_grid|build_module|resource_pcg|ui|gameplay_slice
name:
accepted_target:
forbidden_fallbacks:
owned_contract:
callers:
inputs:
outputs:
phase_or_timing_rules:
edge_cases:
visual_acceptance:
automation_probe:
tests_required:
remaining_questions: []
```

For `unit_type: ui`, include these validator-backed fields:

```yaml
screen_set:
accepted_asset_paths:
required_controls:
data_bindings:
command_dispatch:
visual_acceptance:
automation_probe:
fallback_policy:
```

For `unit_type: gameplay_slice`, include these validator-backed fields:

```yaml
playable_loop:
required_systems:
phase_flow:
player_actions:
feedback_moments:
upgrade_rule:
failure_cases:
visual_readability:
```

For Ocean UI/playability or editor-asset work, `visual_acceptance` must include:

- Asset paths for generated maps, Blueprints, PCG graphs, or widgets.
- Screenshot, UnrealBridge probe, automation test, or log evidence.
- Water, buoyancy, grid, and resource state proof when relevant.
- Explicit fallback policy for cube placeholders or deterministic resource spawn.

## 02 Infra Audit

Classify every requirement:

| Requirement | Status | Evidence | Needed Change |
|---|---|---|---|
| Example | supported|partial|missing|forbidden | File/API/test reference | Narrow change or blocker |

Decision must be one of:

```yaml
decision: use-existing|needs-infra|blocked|forbidden
```

Return to this file when implementation discovers missing bridge APIs, WBP creation gaps, dependency boundary issues, disabled plugins, or untestable behavior.

## 03 Plan

Required sections:

- Files touched.
- `[TDD]` logs to add before C++ implementation.
- Implementation order.
- Rollback/deviation rule.
- Done criteria.
- Non-goals.

For UE C++ work, the plan must explicitly say which `[TDD]` lines are added first and which runner verifies them.

## 04 Tests

Tests must derive from `01-semantics.md`, not from current implementation. Include:

| ID | Test | Method | Expected |
|---|---|---|---|
| T-001 | Semantic behavior | `[TDD]`, static scan, bridge probe, screenshot, or command log | Concrete expected value |

Minimum classes to consider:

- Water profile and actor load.
- Build grid footprint, overlap, rotation, and adjacency.
- Module actor component presence, including buoyancy.
- Resource PCG/fallback spawn determinism and collection.
- UI visibility and clickability, when UI exists.
- Static dependency boundary.
- Regression from stale docs/fallback paths.

## 05 Implementation Log

Record:

- Files changed.
- Planned item completed.
- Deviations from plan and why.
- New infrastructure discovered.
- User edits preserved.

Do not claim a feature is done if tests only prove fallback construction.

## 06 Test Results

Required failure loop fields:

```yaml
result: pass|fail|blocked
failure_type: semantic_mismatch|missing_infra|plan_error|implementation_bug|flaky_tooling|visual_acceptance_gap|none
repro_command:
observed:
expected:
return_gate: 01-semantics|02-infra-audit|03-plan|04-tests|05-implementation
```

Record exact commands and key output. If tests fail, update this file before patching.

## 07 Review

Lead with findings:

| Severity | Finding | File/Line or Evidence | Required Fix |
|---|---|---|---|

Then include:

- Missing tests.
- Boundary/dependency risks.
- Stale document risks.
- Readiness decision: `ship`, `fix`, or `block`.

## Video Method Comparison Checklist

Use this short table before saying the process is "closed":

| Standard | Must Be True |
|---|---|
| Copyable | Another agent can run the same unit from files alone. |
| Parallel-safe | `parallel_lock` prevents contract overlap. |
| Verifiable | Acceptance has script/log/probe/screenshot evidence. |
| Repairable | Failures route back to the correct gate. |
| Low human intervention | Human input is limited to high-risk semantics or taste choices. |
| Playable | A player-visible Ocean behavior is proven, not only internal state. |
