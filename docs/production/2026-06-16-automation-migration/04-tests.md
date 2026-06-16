---
unit_id: 2026-06-16-automation-migration
status: updated
owner: test-designer
updated_at: 2026-06-16T16:35:00
source_commit: working-tree
depends_on: []
parallel_lock: Ocean.EditorAutomation
---

# Tests

| ID | Test | Method | Expected |
|---|---|---|---|
| T-001 | Python scripts parse | `python -m py_compile scripts/ue_tdd_bridge.py scripts/ue_tdd_pipeline.py scripts/harness_state_validator.py scripts/doc_sync_hook.py` | Exit `0`. |
| T-002 | Production state validates | `python scripts/harness_state_validator.py --json` | `success: true`, `error_count: 0`. |
| T-003 | Doc sync runs | `python scripts/doc_sync_hook.py --phase manual --history` | JSON reports `continue: true`, `validator_success: true`. |
| T-004 | No accidental source-project migration | `rg -n "BallGameChaos|Roguelike|ballgame|AutoPlayBuild" ...` | Only intentional explanation text may remain. |
| T-005 | Whitespace diff clean | `git diff --check` | Exit `0`; warnings are acceptable, errors are not. |
| T-006 | Runtime target still builds | `Build.bat Ocean Win64 Development ... -NoHotReload` | Exit `0`. |
| T-007 | Editor target classified | Check active `UnrealEditor` process before `OceanEditor` build. | If editor running and bridge unavailable/new plugin not loaded, report blocked instead of killing. |
