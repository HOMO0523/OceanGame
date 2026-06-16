---
unit_id: 2026-06-16-automation-migration
status: updated
owner: infra-auditor
updated_at: 2026-06-16T16:35:00
source_commit: working-tree
depends_on: []
parallel_lock: Ocean.EditorAutomation
---

# Infra Audit

decision: use-existing

| Requirement | Status | Evidence | Needed Change |
|---|---|---|---|
| UnrealBridge source plugin | supported | Reference plugin is editor-only and source-copyable without binaries. | Copy without `Binaries`, `Intermediate`, `__pycache__`. |
| no-LiveCoding pipeline | supported | Reference `ue_tdd_pipeline.py` auto-detects UE and bridge. | Replace project name, target, and Roguelike console hook. |
| Bridge client | supported | `ue_tdd_bridge.py` is project-agnostic. | Copy directly. |
| Production validator | partial | Validator is generic but lacks Ocean unit schemas. | Add Water/build-grid/module/resource schemas. |
| doc-sync hook | partial | Hook is generic but has source-project description text. | Rename to Ocean. |
| Source gameplay plugins | forbidden | Roguelike plugins encode unrelated gameplay. | Do not migrate. |
