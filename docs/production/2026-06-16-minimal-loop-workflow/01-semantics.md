---
unit_id: 2026-06-16-minimal-loop-workflow
status: frozen
owner: gpt
updated_at: 2026-06-17T00:30:00+08:00
source_commit: 0964ce7
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.MinimalLoopWorkflow
---

# Semantics

unit_type: gameplay_slice
name: Minimal small-boat survival loop planning and automation workflow
playable_loop: spawn on small boat, manage stamina/hydration/satiety, recover with items/rest, store resources, craft modules, place modules on valid boat cells, advance events, survive to the coast ending
required_systems: Ocean-specific Pawn, verified Axis2D WASD movement, Paper2D/HD2D visual character layer, top-down click movement, `F` interaction, survival stats, recovery items, inventory UI, crafting recipes, drag/drop rules, event timeline, boat placement grid, drift progress, ending trigger
phase_flow: GPT reads source docs and freezes semantics; user confirms scope; DS implements through tests; automation saves editor state, closes editor, cold-compiles, relaunches, runs tests, syncs docs, and commits
player_actions: move by left-click or WASD, press `F` for interactions, open inventory/crafting UI, drag items between slots, use food/water, choose event/rest/drift, enter build mode, place craftable modules, reach ending
feedback_moments: status bars, inventory counts, drag/drop highlights, recipe availability, placement preview colors, event result text, day/event counter, game-over/ending screen
upgrade_rule: first craftable upgrades are 1x1 deck, storage box, rain collector, and optional signal item; all placed actors keep placeholder and buoyancy rules
failure_cases: wrong Pawn class, missing Ocean components, W/A/S/D all producing the same Axis2D value, Paper2D visual replacement removing gameplay components, global GameMode launching TopDown template, depleted hydration/satiety, full inventory, insufficient recipe cost, invalid placement cell, non-deterministic random events, no ending trigger
visual_readability: debug-quality UI is acceptable, but the player must see state pressure, inventory/crafting choices, placement validity, event progress, and win/fail result without opening editor details panels
tests_required: C++ automation tests for survival recovery, inventory use/drag model, crafting, event time advance, build allowed areas, drift progress, ending, and Paper2D direction model; UnrealBridge/PIE smoke tests for WASD Axis2D values, Paper2D component retention, UI, and map wiring; no-LiveCoding cold build evidence

## Frozen Interpretation

The current minimum loop remains the small-boat survival loop, not a playable cruise intro. Fishing, diving, island exploration, and full boat physics are deferred. The event system may expose them as disabled or documented future entries, but the code phase must first prove hunger/dehydration recovery, inventory/crafting, placement, event progression, and a complete ending.

WASD movement must be fixed before larger gameplay work: `IA_OceanMove` is an Axis2D action, so `IMC_OceanMVP` must prove `W=(0,+1)`, `S=(0,-1)`, `A=(-1,0)`, and `D=(+1,0)` instead of merely proving those keys exist. The user-observed symptom “all WASD moves right” is treated as an input-asset mapping defect until runtime logs prove otherwise.

Paper2D / HD2D is accepted as the character presentation direction, but only as a visual layer on `BP_OceanSurvivorCharacter`. The gameplay Pawn, collision capsule, Ocean components, interaction, inventory, survival, and build components remain the authority.
