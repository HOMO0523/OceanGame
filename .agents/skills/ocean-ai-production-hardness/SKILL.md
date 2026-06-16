---
name: ocean-ai-production-hardness
description: Ocean prototype production-hardness workflow for turning water, floating platform, modular build-grid, PCG resource, editor automation, and bug-fix work into frozen, testable production units.
---

# Ocean AI Production Hardness

## Core Rule

Turn every non-trivial Ocean request into a reproducible production loop: freeze semantics, audit infrastructure, write tests, implement, run verification, classify failures, patch, rerun, and update memory. Do not let a green compile substitute for a user-visible water/building/resource behavior.

Authority order:

1. `AGENTS.md`
2. `memory-bank/architecture.md`, `memory-bank/progress.md`, `memory-bank/tech-stack.md`
3. `docs/workflows/ocean-agent-workflows.md`
4. Current `docs/superpowers/specs/*` and active `docs/production/*`
5. Older plans only when not contradicted by current authority docs

## Workflow

1. **Read context first.** Load `AGENTS.md`, the three memory-bank files, the relevant spec/plan, and any active production unit.
2. **Choose or create a production unit.** Use `docs/production/{YYYY-MM-DD}-{unit-id}/00..07-*.md`. Reuse an existing unit if it matches the request.
3. **Freeze semantics.** `01-semantics.md` defines `accepted_target`, `forbidden_fallbacks`, edge cases, and executable acceptance.
4. **Audit infrastructure.** `02-infra-audit.md` classifies each requirement as `supported`, `partial`, `missing`, or `forbidden`.
5. **Plan and tests before implementation.** `03-plan.md` names files and order. `04-tests.md` derives tests from semantics, not current code.
6. **Implement under project constraints.** For UE C++ changes, load `ue-tdd-livecoding`, add `[TDD]` logs or automation tests first, then run the no-LiveCoding pipeline.
7. **Verify and classify.** Run C++ automation tests, bridge/PIE checks, static scans, and visual/asset probes as appropriate.
8. **Review and update memory.** `07-review.md` leads with defects and missing tests. Update memory-bank files for architecture/progress/tech-stack changes.

## Ocean Guardrails

| Area | Guardrail |
|---|---|
| Placeholder assets | Missing models use cube placeholders, but the Actor/component contract must match the future real asset. |
| Floating actors | Module/resource/waterborne Actor classes keep `UBuoyancyComponent` or an inherited equivalent. |
| Build modules | A module is a group of grid cells. Never collapse module identity to a single cell unless the definition says `1x1`. |
| Stable grid | Visual bobbing is separate from logical grid coordinates. Grid occupancy must not drift with wave motion. |
| Water setup | `Water` plugin, `WaterBodyCollision`, and water actors are part of acceptance, not optional polish. |
| PCG resources | PCG is the preferred scatter layer; deterministic fallback spawn is allowed only as early infrastructure and must be documented. |
| Editor save gate | Generated maps/assets/PCG graphs/materials must be saved and re-loadable before closing or reporting done. |

## Low-Context Start Checklist

1. Read `AGENTS.md`.
2. Read `docs/design/agent-operating-guide.md`.
3. Read `memory-bank/architecture.md`, `memory-bank/progress.md`, and `memory-bank/tech-stack.md`.
4. Search `docs/superpowers/specs/`, `docs/superpowers/plans/`, and `docs/production/`.
5. Add or identify `[TDD]` probes before C++ implementation.
6. Use UnrealBridge for editor automation when available.
7. If bridge cannot connect and the editor may have unsaved work, stop and report the save-gate blocker.

## Production Unit Acceptance Examples

| Unit Type | Acceptance |
|---|---|
| `water_setup` | Water plugin enabled, `WaterBodyCollision` present, test map loads WaterZone/WaterBodyOcean, PIE has no water collision errors. |
| `build_grid` | Footprint math, overlap rejection, rotation, and adjacency tests pass; visual platform remains stable. |
| `build_module` | Module definition has footprint/cost/actor class; spawned actor has placeholder mesh and buoyancy. |
| `resource_pcg` | PCG component/graph exists or fallback scatter is deterministic; resources spawn outside exclusion radius and are collectable. |
| `editor_automation` | Pipeline can save, close, build, launch, run PIE, capture logs, and analyze `[TDD]` lines. |

## Commands

```powershell
python scripts/harness_state_validator.py --json
python scripts/doc_sync_hook.py --phase manual
python scripts/ue_tdd_pipeline.py --pie-duration 5
python scripts/ue_tdd_pipeline.py --check-only
git diff --check
```

For UE C++ changes, use `scripts/ue_tdd_pipeline.py` or the `ue-tdd-livecoding` skill's full save/close/build/launch/PIE/capture/analyze loop. Do not use Live Coding or Hot Reload as compile proof.

## Required Reference

Read `references/production-unit-templates.md` when creating or repairing a production unit, writing subagent outputs, defining visual/asset acceptance, or classifying test failures.

## Stop Conditions

Ask the user only when a frozen semantic target is genuinely ambiguous, a missing infrastructure choice changes scope, or a visual/product-taste decision cannot be inferred from existing docs. Otherwise advance the loop and record decisions in state files.
