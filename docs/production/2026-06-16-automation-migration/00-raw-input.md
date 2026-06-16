---
unit_id: 2026-06-16-automation-migration
status: draft
owner: intake
updated_at: 2026-06-16T16:35:00
source_commit: working-tree
depends_on: []
parallel_lock: Ocean.EditorAutomation
---

# Raw Input

User requested learning the automation chain and plugins from the reference Unreal project at `{ExternalRefsRoot}/NWUEBallGame` and migrating the reusable contents into Ocean.

## Interpreted Scope

- Copy only generic editor automation and production workflow infrastructure.
- Adapt copied items to Ocean project names and Ocean phase-one rules.
- Do not copy Roguelike gameplay plugins, economy scripts, balance analyzers, or UI semantics.
