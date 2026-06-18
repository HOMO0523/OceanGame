---
unit_id: 2026-06-18-mvp-validation-hardening
status: active
owner: test-designer
updated_at: 2026-06-18T14:05:00+08:00
source_commit: 486a9d5
depends_on: [2026-06-16-mvp-survival-loop, 2026-06-17-hud-backpack-drawer-uiux]
parallel_lock: Ocean.MVPValidation
---

# Tests

| ID | Test | Method | Expected |
|---|---|---|---|
| T-001 | Resource node contract | `Automation RunTests Ocean.Resources.Node.HasPlaceholderAndBuoyancy` | Placeholder mesh, buoyancy component, and physics/buoyancy configuration pass without requiring a registered world unless the test spawns one. |
| T-002 | Backpack refresh crash regression | `Automation RunTests Ocean.UI.BackpackPanel.RefreshSlots` | Commandlet completes without crash; adding `fresh_water` then refreshing keeps slot model intact. |
| T-003 | Backpack panel visibility | `Automation RunTests Ocean.UI.PanelVisibility.TakeWidget` | Runtime panels return non-null Slate widgets and set root widgets. |
| T-004 | Project automation suite | Sequential commandlets for `Ocean.Build`, `Ocean.Resources`, `Ocean.UI`, `Ocean.MVP`, and `Ocean.Paper2D` | All project-owned tests pass; bare `Automation RunTests Ocean` is excluded because it also matches UE Water plugin tests. |
| T-005 | MVP asset verifier | Bridge `scripts/verify_mvp_survival_loop.py` | Current input mappings, GameMode class, map actors, Paper2D, and starter resources match current accepted contracts. |
| T-006 | UI asset verifier | Bridge `scripts/verify_ocean_ui_assets.py` | Ten WBP assets load, HUD class binding passes, WaterOcean map loads. |
| T-007 | HUD PIE verifier | `python scripts/verify_ocean_ui_pie.py`, then Bridge `scripts/verify_ocean_ui_assets.py --pie` | External probe reaches gameplay map, captures real `OceanHUDRootPIE: created=1 drawer_open=0`, and asset verifier confirms the log. |
| T-008 | Cold build + PIE smoke | `python scripts/ue_tdd_pipeline.py --pie-duration 5 --log-lines 24000` | Save gate, cold compile, editor launch, PIE, and `[TDD]` capture complete. |
| T-009 | Static hygiene | `harness_state_validator`, `doc_sync_hook`, `git diff --check` | Docs valid, doc sync clean, no whitespace errors. |
| T-010 | Snapshot-slot UI mode | `Automation RunTests Ocean.UI.SaveSlot.SnapshotTargetMode` | Pause-menu save-slot rows can enable empty slots as snapshot targets instead of treating them only as existing load slots. |
| T-011 | Saved inventory restore | `Automation RunTests Ocean.MVP.Inventory.RestoreState` | Saved resource stacks and item slots restore into the runtime inventory with reindexed slots. |
| T-012 | Dive camera toggle | `Automation RunTests Ocean.MVP.Dive.CameraToggle` | X dive enters underwater walking mode, shortens camera arm, and a second X exits dive while restoring the original camera arm. |
| T-013 | Snapshot slot clamp | `Automation RunTests Ocean.MVP.Save.SnapshotSlotClamp` | The active day-snapshot target is clamped to the supported three-slot range. |
