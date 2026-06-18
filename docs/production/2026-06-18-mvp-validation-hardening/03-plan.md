---
unit_id: 2026-06-18-mvp-validation-hardening
status: active
owner: planner
updated_at: 2026-06-18T14:05:00+08:00
source_commit: 486a9d5
depends_on: [2026-06-16-mvp-survival-loop, 2026-06-17-hud-backpack-drawer-uiux]
parallel_lock: Ocean.MVPValidation
---

# Plan

1. Preserve baseline evidence and keep the checkpoint on remote.
2. Repair the first targeted red test: resource-node placeholder/buoyancy/physics contract.
3. Repair the second targeted red test: backpack panel `TakeWidget` / refresh crash.
4. Re-run targeted tests before broader tests.
5. Update stale Bridge verifiers to current MVP contracts.
6. Add or repair game-map HUD PIE smoke so main menu and playable map are both testable.
7. Run full static, C++ automation, Bridge, cold-build, and PIE validation.
8. Record failures, fixes, warnings, and remaining deferred MVP work.
9. Commit and push only intended files.

Initial root-cause hypotheses to test:

- `OceanResource_PhysicsEnabled` fails because the test checks `IsSimulatingPhysics()` on an unregistered `NewObject` actor even though constructor configuration sets `SetSimulatePhysics(true)`.
- `Ocean.UI.BackpackPanel.RefreshSlots` crashes because commandlet tests created `UUserWidget` via `NewObject` and called `TakeWidget()` before `Initialize()`, leaving `WidgetTree` null.
- `verify_ocean_ui_assets.py --pie` fails when no prior gameplay-map PIE log exists; active PIE probing must be driven from an external Bridge client so the editor can tick.
