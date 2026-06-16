---
unit_id: 2026-06-16-automation-migration
status: updated
owner: test-runner
updated_at: 2026-06-16T16:35:00
source_commit: working-tree
depends_on: []
parallel_lock: Ocean.EditorAutomation
---

# Test Results

result: blocked
failure_type: flaky_tooling
repro_command: see commands below
observed: non-editor checks pass; `OceanEditor` compile is blocked by a running editor with Live Coding active
expected: non-editor checks pass; editor compile runs after save gate or editor restart
return_gate: 06-test-results

## Results

| ID | Command | Result |
|---|---|---|
| T-001 | `python -m py_compile .\scripts\ue_tdd_bridge.py .\scripts\ue_tdd_pipeline.py .\scripts\harness_state_validator.py .\scripts\doc_sync_hook.py` | PASS, exit `0`. |
| T-002 | `python .\scripts\harness_state_validator.py --json` | PASS, `success: true`, `error_count: 0`, `unit_count: 1`. |
| T-003 | `python .\scripts\doc_sync_hook.py --phase manual --history` | PASS, `continue: true`, `validator_success: true`. |
| T-004 | `rg -n "BallGameChaos|Roguelike|ballgame|AutoPlayBuild" ...` | PASS with only intentional exclusion notes in docs. |
| T-005 | `git diff --check` | PASS, no whitespace errors; Git reports CRLF conversion warnings only. |
| T-006 | `Build.bat Ocean Win64 Development ... -NoHotReload` | PASS, `Result: Succeeded`. |
| T-007 | `Build.bat OceanEditor Win64 Development ... -NoHotReload` | BLOCKED, `Unable to build while Live Coding is active`. |

## Editor Target Blocker

`OceanEditor` reaches UHT/makefile processing and then fails because an `UnrealEditor.exe` process is running with Live Coding active. This is an expected save-gate blocker. Do not kill the editor unless the user confirms unsaved work is safe.

The migrated `UnrealBridge` now declares its `StructUtils` plugin dependency. UE reports `StructUtils` is deprecated in 5.5+, but the reference bridge source still links the module.
