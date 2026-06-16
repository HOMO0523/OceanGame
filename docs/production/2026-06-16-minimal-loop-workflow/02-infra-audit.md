---
unit_id: 2026-06-16-minimal-loop-workflow
status: audited
owner: gpt
updated_at: 2026-06-16T23:58:00+08:00
source_commit: 5af8141
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.MinimalLoopWorkflow
---

# Infrastructure Audit

| Requirement | Current Support | Gap / Risk |
|---|---|---|
| Ocean Pawn | `BP_OceanSurvivorCharacter` is the verified MVP Pawn and inherits Ocean gameplay components. | `BP_TopDownCharacter` remains a confusing template reference; global defaults may still point to TopDown assets. |
| Movement | Left-click and WASD are already part of MVP semantics. | Need preserve both when adding UI modes and placement. |
| Survival stats | `UOceanSurvivalComponent` tracks stamina, hydration, and satiety. | Missing item recovery, fail state, event-driven drain tuning. |
| Inventory | `UOceanInventoryComponent` supports stack add/spend/capacity. | Missing user-facing UI, use-item action, drag/drop model. |
| Crafting | Build cost exists for deck placement. | Missing standalone recipe data, craft output, recipe UI. |
| Placement | Build grid, adjacency, occupied-cell rejection, and buoyant module actors exist. | Missing preview UX and more module types/allowed-area explanations. |
| Events | No formal event timeline system yet. | Need 7-day/21-event progression, seeded randomness, result application. |
| Boat control | Floating platform exists. | Real boat physics should be deferred; need drift progress/heading event abstraction. |
| Ending | No complete ending trigger yet. | Need survive-to-coast result and failure state. |
| Automation | UnrealBridge, TDD pipeline, production validator, doc sync, defense docs exist. | Need a GPT-to-DS handoff gate and tests for the next systems. |
