---
unit_id: 2026-06-16-mvp-survival-loop
status: captured
owner: planner
updated_at: 2026-06-16T18:52:00
source_commit: 065269f
depends_on: [2026-06-16-water-ocean-bootstrap]
parallel_lock: Ocean.MVPSurvivalLoop
---

# Raw Input

The first playable MVP should prioritize two tracks:

1. Small-boat survival loop: skip playable cruise intro; start with initial supplies; build the ocean boat/platform scene, three survival stats, inventory, `F` interaction, and resource pickup.
2. Construction-growth loop: platform expansion, floating modules, and resource scattering stay in scope as the technical/growth layer.

Confirmed user constraints:

- Keep left-click floor movement from the TopDown template.
- Add WASD movement; pressing WASD interrupts any current click-to-move path.
- Add `F` as the unified interaction key.
- Fishing and diving abilities/interactions are deferred from this unit and listed as future work only.
- Missing models use cube placeholders.
- Waterborne actor assets must keep a buoyancy path.

