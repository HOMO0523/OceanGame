---
unit_id: 2026-06-16-mvp-survival-loop
status: frozen
owner: planner
updated_at: 2026-06-16T18:52:00
source_commit: 065269f
depends_on: [2026-06-16-water-ocean-bootstrap]
parallel_lock: Ocean.MVPSurvivalLoop
---

# Semantics

unit_type: gameplay_slice
name: Small-boat survival and light construction loop
playable_loop: spawn on small ocean platform, move, pick up floating resources, watch survival stats, spend resources on one deck expansion, repeat
required_systems: top-down click movement, WASD movement, `F` interaction, stack inventory, survival component, debug HUD, build grid placement, resource field
phase_flow: start with initial supplies and nearby floating resources; normal mode handles movement/pickup; build mode spends resources and expands platform
player_actions: left-click move, WASD move, `F` interact, `B` toggle build mode, `R` rotate build preview, left-click confirm build while in build mode
feedback_moments: HUD stat bars/text, inventory counts, interaction prompt, build success/failure log, pickup success/failure message
upgrade_rule: first upgrade is a `1x1` deck module placed through existing grid/module rules and paid from inventory wood
accepted_map_paths: `/Game/OceanPrototype/Maps/L_WaterOcean`
primary_player_class: `BP_OceanSurvivorCharacter`, derived from the project top-down character path
movement_contract: left-click movement remains available; WASD adds camera-relative movement; WASD interrupts current click-to-move
interaction_contract: `F` chooses the nearest valid interactable in range and reports failure reasons
survival_contract: stamina, hydration, and satiety are visible, tick down or recover according to simple first-pass rules, and can be read by UI/tests
inventory_contract: stack-based inventory accepts wood, scrap/plastic, food, and water; full or insufficient resources produce feedback
build_contract: place at least one `1x1` deck/platform module through the existing grid/module rules; module keeps buoyancy path
resource_contract: floating resources spawn near but not inside the platform safe area; pickup adds to inventory; resources keep buoyancy path
deferred_features: fishing, diving, island exploration, playable cruise intro, complex status effects, save/load, multiplayer
failure_cases: no interactable in range, inventory full, insufficient resources, invalid build cell, missing input mapping, missing map actors, missing buoyancy component
visual_readability: debug HUD must make stats, inventory counts, interaction prompt, and build mode visible during PIE without opening editor details panels
tests_required: C++ automation tests for input/inventory/survival/interaction/build; bridge verification for map actors/assets; no-LiveCoding build and PIE log capture

## Accepted Target

The player can launch `L_WaterOcean`, move on the starter platform with either left-click pathing or WASD, see survival state and inventory information, pick up at least one floating resource with `F`, and place at least one platform expansion module by spending resources.

## Forbidden Fallbacks

- Do not remove or break existing left-click movement.
- Do not make left-click resource pickup part of normal movement mode.
- Do not implement fishing or diving inside this unit.
- Do not fake waterborne modules/resources without a buoyancy component or inherited buoyancy path.
- Do not allow build grid coordinates to drift with visual bobbing.
- Do not rely on Live Coding or Hot Reload for C++ verification.
