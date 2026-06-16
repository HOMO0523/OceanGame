---
unit_id: 2026-06-16-mvp-survival-loop
status: planned
owner: planner
updated_at: 2026-06-16T18:52:00
source_commit: 065269f
depends_on: [2026-06-16-water-ocean-bootstrap]
parallel_lock: Ocean.MVPSurvivalLoop
---

# Plan

Canonical implementation plan: `docs/superpowers/plans/2026-06-16-ocean-mvp-survival-loop.md`.

Execution order:

1. Add C++ automation tests for input math, survival ticking, inventory stack behavior, interaction priority, and build placement.
2. Add focused C++ runtime components under `Source/Ocean/OceanPrototype`.
3. Extend `AOceanPlayerController` without removing click-to-move.
4. Add simple HUD and build interaction path.
5. Add editor automation script to create Ocean MVP assets and place actors in `L_WaterOcean`.
6. Run no-LiveCoding build and bridge/PIE checks.
7. Update `memory-bank` progress/architecture and append defense commit log.

