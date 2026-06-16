---
unit_id: 2026-06-16-water-ocean-bootstrap
status: frozen
owner: clarifier
updated_at: 2026-06-16T16:50:00
source_commit: working-tree
depends_on: [2026-06-16-automation-migration]
parallel_lock: Ocean.WaterSetup
---

# Semantics

unit_type: water_setup
name: First WaterOcean map
accepted_map_paths: `/Game/OceanPrototype/Maps/L_WaterOcean`
required_plugins: `Water`, `EditorScriptingUtilities`; `UnrealBridge` for future live editor automation
collision_profile: `WaterBodyCollision` in `Config/DefaultEngine.ini`
water_actors: one `AWaterZone` labeled `WaterZone_Prototype`, one `AWaterBodyOcean` labeled `WaterBodyOcean_Prototype`
buoyancy_probe: later floating actors must use `UBuoyancyComponent`; this unit only creates the water body that buoyancy will sample
save_gate: created/updated map must run `save_dirty_packages(True, True)` and reload/existence verification
failure_cases: missing Water plugin, missing collision profile, running editor with no bridge, Python remote disabled, editor Live Coding blocking cold editor compile
tests_required: script syntax, Water profile scan, editor automation access check, map actor count `[TDD]` logs, save result

## Accepted Target

`L_WaterOcean` is a simple prototype level that contains a large WaterZone, a WaterBodyOcean, and minimal visibility helpers. It does not yet include final islands, platform art, PCG graph assets, or build-mode input.

## Forbidden Fallbacks

- Do not create a fake plane mesh and call it ocean.
- Do not use Live Coding as proof.
- Do not open a second editor/commandlet against the same project while an unsaved editor may be open.
- Do not kill the current editor unless the user explicitly accepts unsaved-work risk.
