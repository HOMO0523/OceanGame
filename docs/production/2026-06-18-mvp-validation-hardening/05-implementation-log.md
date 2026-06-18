---
unit_id: 2026-06-18-mvp-validation-hardening
status: active
owner: implementer
updated_at: 2026-06-18T14:05:00+08:00
source_commit: 486a9d5
depends_on: [2026-06-16-mvp-survival-loop, 2026-06-17-hud-backpack-drawer-uiux]
parallel_lock: Ocean.MVPValidation
---

# Implementation Log

- 2026-06-18 14:05: Created this production unit after checkpoint push `486a9d5`.
- Incoming red evidence: full `Automation RunTests Ocean` failed resource-node test and crashed in backpack panel refresh test.
- 2026-06-18 14:17: Repaired `Ocean.Resources.Node.HasPlaceholderAndBuoyancy` by checking constructor-configured `BodyInstance.bSimulatePhysics` for the unregistered resource actor instead of runtime `IsSimulatingPhysics()`.
- 2026-06-18 14:18: Reproduced backpack commandlet crash as `Object is not packaged: Border BgBorder_0`; root cause was `NewObject<UUserWidget>` tests calling `TakeWidget()` before `Initialize()`, leaving `WidgetTree` null.
- 2026-06-18 14:23: Updated backpack/status/visibility UI tests to call `Initialize()` before `TakeWidget()` and corrected StatusPanel float tolerance plus root-background border count.
- 2026-06-18 14:25: Project-owned automation groups passed: `Ocean.Build`, `Ocean.Resources`, `Ocean.UI`, `Ocean.MVP`, and `Ocean.Paper2D`. Bare `Automation RunTests Ocean` is rejected as an acceptance command because it also runs UE Water plugin tests with “Ocean” in their names.
- 2026-06-18 14:31: Synced MVP verifier to current contracts: 13 input mappings, `IA_OceanToggleBackpack` Tab/I, `AOceanMVPGameMode`, and `PlayerStart_WaterOcean`; synced setup script constants too.
- 2026-06-18 14:36: Added `scripts/verify_ocean_ui_pie.py` because PIE must be driven from the external Bridge client; sleeping inside Unreal Python blocks editor ticks and can false-fail the HUD probe.
- 2026-06-18 19:27: Added red tests for inventory restore, snapshot-slot target mode, and X dive camera toggling; initial cold compile failed on missing `RestoreInventoryState` and `SetSnapshotTargetMode`.
- 2026-06-18 19:33: Implemented day-snapshot slot selection, cross-day snapshot writes, inventory restore, safe load-to-surface behavior, pause cursor restoration, and X dive camera arm down/up restore.
- 2026-06-18 19:36: Fixed the dive automation test world setup after a duplicate `WorldSettings` commandlet crash; `Ocean.MVP.Dive.CameraToggle` then passed.
- 2026-06-18 19:38: Added `Ocean.MVP.Save.SnapshotSlotClamp` and corrected the subsystem test outer to `UGameInstance`.
