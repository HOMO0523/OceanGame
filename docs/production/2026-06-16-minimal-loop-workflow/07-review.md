---
unit_id: 2026-06-16-minimal-loop-workflow
status: reviewed
owner: gpt
updated_at: 2026-06-17T16:20:00+08:00
source_commit: pending
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.MinimalLoopWorkflow
---

# Review

## Review Focus

- The spec has entered its first code-stage slice after user confirmation.
- The Pawn discussion protects existing Ocean gameplay components while preserving TopDown movement semantics.
- The minimal loop prioritizes recovery, inventory, crafting, events, placement, drift progress, and ending over large-scene expansion.
- The user-observed WASD bug was proven at the input mapping layer and fixed by adding per-key Axis2D modifiers to `IMC_OceanMVP`.
- Paper2D / HD2D is now an experimental visual layer on the Ocean survivor Pawn, not a replacement for the gameplay Pawn.
- Fishing, full diving, island exploration, playable cruise intro, and real boat physics remain deferred until the small loop is complete.
- `SpaceBar` jump is active; `E` is only a water-edge dive-entry gate and does not yet create underwater gameplay.

## Follow-Up Gate

Next DS/code slice should focus on either survival recovery / inventory UX or Paper2D state switching; do not expand fishing, full diving, or island travel before the small loop is stable.
