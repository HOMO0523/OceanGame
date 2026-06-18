---
unit_id: 2026-06-18-mvp-validation-hardening
status: active
owner: clarifier
updated_at: 2026-06-18T14:05:00+08:00
source_commit: 486a9d5
depends_on: [2026-06-16-mvp-survival-loop, 2026-06-17-hud-backpack-drawer-uiux]
parallel_lock: Ocean.MVPValidation
---

# Semantics

unit_type: infra
name: MVP test and landing hardening
capability: Repeatable Ocean MVP validation chain covering cold build, project C++ automation prefixes, Bridge asset verification, gameplay-map HUD PIE evidence, day-snapshot save boundaries, and MVP dive-camera toggling.
accepted_target: A repeatable validation chain that proves the Ocean MVP foundation can build, load critical assets, run C++ automation tests, reach playable HUD in PIE, preserve day-snapshot save semantics, prove X dive camera restore, and record unresolved gameplay gaps without false-green reports.
forbidden_fallbacks: Claiming MVP complete from compile only; skipping failing automation; masking crashes by deleting tests; silently pushing temp scripts; using Live Coding or Hot Reload; requiring manual editor clicks for the validation path.
forbidden_dependencies: Manual editor clicking, Live Coding, Hot Reload, bare `Automation RunTests Ocean`, stale logs without a fresh external PIE probe, or staging temporary debug scripts as deliverables.
owned_contract: This unit owns test repair, verifier repair, documented blockers, MVP smoke coverage, day-snapshot save/load semantics, and the minimal X dive/camera traversal slice. It does not expand gameplay scope beyond the current small-boat loop.
callers: `scripts/ue_tdd_pipeline.py`, `scripts/ue_tdd_bridge.py`, C++ automation tests under `Source/Ocean/Tests`, `scripts/verify_mvp_survival_loop.py`, `scripts/verify_ocean_ui_assets.py`, `scripts/verify_ocean_ui_pie.py`.
inputs: Cold build command, commandlet automation tests, Bridge Python verifiers, PIE logs, main-menu/new-game flow, game-map HUD flow.
outputs: Passing or explicitly classified verification results; source/test fixes for root-cause failures; updated production docs and memory-bank progress.
phase_or_timing_rules: Run cheap static checks first, then targeted failing tests, then cold compile, then full automation, then Bridge/PIE verifiers. Full 7-day runtime may use a fast deterministic test instead of a 420-second manual wait.
edge_cases: Main menu default map, stale generated assets, unregistered actor components in unit tests, UMG `TakeWidget` in commandlet tests, world-spawned vs `NewObject` actors, old verifier expectations, unsaved editor packages.
visual_acceptance: PIE can create the gameplay HUD root on `L_WaterOcean` and emit the real `[TDD] OceanHUDRootPIE: created=1 drawer_open=0` log; the main-menu map can still show the menu without blocking game-map verification.
automation_probe: Project-owned automation prefixes (`Ocean.Build`, `Ocean.Resources`, `Ocean.UI`, `Ocean.MVP`, `Ocean.Paper2D`) complete without crash; `Automation RunTests Ocean` is not an acceptance command because it also matches UE Water plugin tests containing “Ocean”; `verify_mvp_survival_loop.py` passes current asset contracts; `verify_ocean_ui_pie.py` reaches the game map and `verify_ocean_ui_assets.py --pie` can confirm the resulting HUD log; `ue_tdd_pipeline.py` can cold-build and run PIE; `Ocean.MVP.Save` proves snapshot-slot selection; `Ocean.MVP.Dive` proves camera arm down/up restore.
tests_required: `Ocean.Resources.Node.HasPlaceholderAndBuoyancy`, `Ocean.UI.BackpackPanel.RefreshSlots`, `Ocean.UI.SaveSlot.SnapshotTargetMode`, `Ocean.MVP.Inventory.RestoreState`, `Ocean.MVP.Save.SnapshotSlotClamp`, `Ocean.MVP.Dive.CameraToggle`, `Ocean.MVP.*`, `Ocean.UI.*`, `Ocean.Paper2D.*`, `BridgeVerifyMVP`, `BridgeVerifyOceanUIAssets`, `BridgeVerifyOceanHUDPIE`, `git diff --check`, `harness_state_validator`.
remaining_questions: []

deferred_scope: Final art, full fishing minigame, oxygen, underwater rewards/collection, island scene travel, cruise intro, and balancing are not required for this unit.
