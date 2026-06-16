---
unit_id: 2026-06-16-automation-migration
status: updated
owner: planner
updated_at: 2026-06-16T16:35:00
source_commit: working-tree
depends_on: []
parallel_lock: Ocean.EditorAutomation
---

# Plan

## Files Touched

- Create `Plugins/UnrealBridge/**` source-only copy.
- Create `scripts/ue_tdd_bridge.py`, `scripts/ue_tdd_pipeline.py`, `scripts/harness_state_validator.py`, `scripts/doc_sync_hook.py`.
- Create `AGENTS.md`, `docs/design/agent-operating-guide.md`, `memory-bank/*.md`.
- Create `.agents/skills/ue-tdd-livecoding`, `.agents/skills/unreal-bridge`, `.agents/skills/ocean-ai-production-hardness`.
- Modify `Ocean.uproject` to enable `UnrealBridge` for Editor.
- Modify `docs/workflows/ocean-agent-workflows.md` to mark migrated automation.

## Implementation Order

1. Copy source-only UnrealBridge and generic scripts.
2. Ocean-adapt pipeline constants and console command behavior.
3. Add project authority docs and memory-bank.
4. Add Ocean-specific local skills.
5. Add production-unit validator schemas.
6. Run verification commands and classify editor-build blocker if present.

## Done Criteria

- Python scripts compile.
- Production validator returns `success: true`.
- Doc-sync hook writes a report.
- `git diff --check` reports no errors.
- Game target build succeeds, or a concrete blocker is recorded.
- Editor target is either verified or blocked by an active editor/save-gate condition.
