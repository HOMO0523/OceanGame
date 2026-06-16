---
unit_id: 2026-06-16-minimal-loop-workflow
status: planned
owner: gpt
updated_at: 2026-06-16T23:58:00+08:00
source_commit: 5af8141
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.MinimalLoopWorkflow
---

# Plan

## Docs-Only Step

1. Read project rules, memory-bank docs, existing MVP production docs, and DOCX source.
2. Analyze Pawn class split and current Ocean MVP component dependencies.
3. Write the formal minimal-loop spec and workflow document.
4. Record this planning unit in `docs/production`.
5. Validate docs and commit/push.

## Code-Stage Handoff

When the user confirms the spec and chooses DS:

1. Create or update a detailed implementation plan under `docs/superpowers/plans`.
2. Add failing automation tests for each subsystem before implementation.
3. Implement one subsystem per small commit.
4. Use the no-LiveCoding save/close/build/relaunch/PIE pipeline.
5. Sync memory-bank, production docs, defense HTML, and commit log after verified changes.

## Recommended Implementation Order

1. Survival recovery and failure state.
2. Inventory UI and drag/drop data model.
3. Crafting recipes and output flow.
4. Placement preview and allowed-area UX.
5. Event timeline and drift progress.
6. Ending / game-over screens.
