---
unit_id: 2026-06-16-minimal-loop-workflow
status: frozen
owner: gpt
updated_at: 2026-06-16T23:58:00+08:00
source_commit: 5af8141
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.MinimalLoopWorkflow
---

# Semantics

unit_type: gameplay_slice
name: Minimal small-boat survival loop planning and automation workflow
playable_loop: spawn on small boat, manage stamina/hydration/satiety, recover with items/rest, store resources, craft modules, place modules on valid boat cells, advance events, survive to the coast ending
required_systems: Ocean-specific Pawn, top-down click movement, WASD movement, `F` interaction, survival stats, recovery items, inventory UI, crafting recipes, drag/drop rules, event timeline, boat placement grid, drift progress, ending trigger
phase_flow: GPT reads source docs and freezes semantics; user confirms scope; DS implements through tests; automation saves editor state, closes editor, cold-compiles, relaunches, runs tests, syncs docs, and commits
player_actions: move by left-click or WASD, press `F` for interactions, open inventory/crafting UI, drag items between slots, use food/water, choose event/rest/drift, enter build mode, place craftable modules, reach ending
feedback_moments: status bars, inventory counts, drag/drop highlights, recipe availability, placement preview colors, event result text, day/event counter, game-over/ending screen
upgrade_rule: first craftable upgrades are 1x1 deck, storage box, rain collector, and optional signal item; all placed actors keep placeholder and buoyancy rules
failure_cases: wrong Pawn class, missing Ocean components, global GameMode launching TopDown template, depleted hydration/satiety, full inventory, insufficient recipe cost, invalid placement cell, non-deterministic random events, no ending trigger
visual_readability: debug-quality UI is acceptable, but the player must see state pressure, inventory/crafting choices, placement validity, event progress, and win/fail result without opening editor details panels
tests_required: C++ automation tests for survival recovery, inventory use/drag model, crafting, event time advance, build allowed areas, drift progress, and ending; UnrealBridge/PIE smoke tests for UI and map wiring; no-LiveCoding cold build evidence

## Frozen Interpretation

The current minimum loop remains the small-boat survival loop, not a playable cruise intro. Fishing, diving, island exploration, and full boat physics are deferred. The event system may expose them as disabled or documented future entries, but the code phase must first prove hunger/dehydration recovery, inventory/crafting, placement, event progression, and a complete ending.
