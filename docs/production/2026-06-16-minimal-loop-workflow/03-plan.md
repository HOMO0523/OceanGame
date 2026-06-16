---
unit_id: 2026-06-16-minimal-loop-workflow
status: planned
owner: gpt
updated_at: 2026-06-17T00:30:00+08:00
source_commit: 0964ce7
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

0. WASD Axis2D mapping fix and runtime input-vector probes.
1. Paper2D / HD2D visual layer on `BP_OceanSurvivorCharacter`.
2. Survival recovery and failure state.
3. Inventory UI and drag/drop data model.
4. Crafting recipes and output flow.
5. Placement preview and allowed-area UX.
6. Event timeline and drift progress.
7. Ending / game-over screens.

## WASD Root-Cause Gate

Before changing movement code, DS should prove where the bad value enters the system:

1. Inspect `IMC_OceanMVP` mappings for W/A/S/D Axis2D modifiers or equivalent generated values.
2. Add a Bridge or PIE `[TDD]` probe that logs `InputVector` and `WorldDirection`.
3. If W/A/S/D all emit `(1,0)`, fix the input mapping asset/script, not `FOceanInputMath`.
4. Only change controller code if the mapping emits correct vectors but world movement is still wrong.

## Paper2D Gate

Paper2D work starts after the input gate:

1. Enable/verify Paper2D availability.
2. Add a Paper2D visual component to the Ocean survivor Pawn.
3. Hide or replace the visible template mesh without removing Ocean gameplay components.
4. Add direction/animation selection as a visual model that reads movement direction but does not own movement.
