---
unit_id: 2026-06-16-minimal-loop-workflow
status: implemented
owner: gpt
updated_at: 2026-06-17T16:20:00+08:00
source_commit: pending
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.MinimalLoopWorkflow
---

# Implementation Log

- 2026-06-16: Read project authority docs, current MVP production unit, workflow docs, source/controller references, and `洋流.docx`.
- 2026-06-16: Confirmed `BP_OceanSurvivorCharacter` is the correct Ocean MVP Pawn target; `BP_TopDownCharacter` should remain a template reference unless explicitly revalidated.
- 2026-06-16: Added a docs-only minimal-loop spec and GPT/DS handoff workflow.
- 2026-06-17: Entered the code-stage handoff described in `03-plan.md`.
- 2026-06-17: Proved the WASD bug at the input mapping layer: `IMC_OceanMVP` had W/A/S/D keys but no Axis2D modifiers, so every key emitted the same movement direction.
- 2026-06-17: Fixed `scripts/setup_mvp_survival_loop.py` and the generated `IMC_OceanMVP` so W/A/S/D emit distinct vectors: W up, A left, S down, D right; left-click and touch movement remain present.
- 2026-06-17: Added `IA_OceanJump` on `SpaceBar` and `IA_OceanDive` on `E`.
- 2026-06-17: Added water-edge predicates to `UOceanBuildGridComponent` and `AOceanFloatingPlatform`; `E` only succeeds when the player stands on a platform cell adjacent to unoccupied water.
- 2026-06-17: Added jump/dive bindings to `AOceanPlayerController`, including `[TDD]` logs for jump and dive-entry result.
- 2026-06-17: Kept the agreed scope boundary: fishing and full diving remain deferred; current dive work is only an entry gate for future event/underwater systems.
