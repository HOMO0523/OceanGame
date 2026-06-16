---
unit_id: 2026-06-16-minimal-loop-workflow
status: captured
owner: gpt
updated_at: 2026-06-16T23:58:00+08:00
source_commit: 5af8141
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.MinimalLoopWorkflow
---

# Raw Input

User requested a docs-first planning pass for 《洋流》:

- Analyze why the player Pawn is not `BP_TopDownCharacter` and whether this has hidden risk.
- Read `C:\Users\shxuw\Desktop\洋流.docx`.
- Discuss the minimum loop: hunger, dehydration, recovery, inventory, crafting, drag-drop, events, placeable areas, boat control, and an ending.
- Build a complete automation workflow where GPT owns detailed layout/spec/TDD strategy, then stops before code so the user can choose DS for implementation.
- Write documentation now; do not enter code implementation in this step.

The DOCX extraction is stored as a local report at `Saved/HarnessReports/20260616-ocean-docx-extract.txt`.
