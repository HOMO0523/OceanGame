---
unit_id: 2026-06-17-paperzd-pie-visibility
status: frozen
owner: debugger
updated_at: 2026-06-17T12:55:00+08:00
source_commit: 2c9e304
depends_on: [2026-06-17-paper2d-state-machine]
parallel_lock: Ocean.Paper2DVisibility
---

# Semantics

unit_type: infra
name: PaperZD enablement and PIE spawn visibility fix
capability: Enable PaperZD safely while preserving the existing Ocean Paper2D pawn path, and make UnrealBridge PIE spawn respect authored map PlayerStart actors.
owned_contract: UnrealBridge owns automated PIE spawn policy; Ocean Paper2D animation component owns Flipbook state selection only; Blueprint/component defaults own sprite transform angle.
callers: `scripts/ue_tdd_pipeline.py`, UnrealBridge editor library, Codex agents, and human PIE test sessions.
forbidden_dependencies: Live Coding as compile proof, PaperZD AnimBP wiring in this slice, deleting `BP_OceanSurvivorCharacter`, forcing editor-camera spawn when PlayerStart actors exist.
accepted_target: PIE should spawn the Ocean survivor at the map PlayerStart when one exists, so the camera follows the playable pawn and the Paper2D/PaperZD visual layer is visible in the MVP scene.
forbidden_fallbacks: blaming PaperZD without runtime evidence; deleting existing Paper2D MVP components; forcing a manual editor click as the only workaround.
observed_failure: Bridge PIE creates `PlayerStartPIE_0` from the active editor camera; the pawn spawns far away and settles under the ocean/landscape, so the visible Paper2D component is outside readable view/under water.
PaperZD_boundary: PaperZD is enabled and classes are registered, but no PaperZD animation blueprint/component has been wired into `BP_OceanSurvivorCharacter` yet. Existing Ocean Paper2D component still owns visuals.
acceptance: Bridge `StartPIE` must use map PlayerStart when PlayerStart actors exist; a TDD log must report PASS for this path; PIE probe should show no `PlayerStartPIE` spawn override for normal MVP maps.
manual_angle_decision: automatic camera-facing Paper2D rotation was removed by user decision; `Paper2DVisual` angle remains manually adjustable in Blueprint/component defaults.
direction_decision: current Paper2D rows require W/S mapped to up/down and A/D rows swapped for screen-left/screen-right readability.
tests_required: harness validator, doc sync hook, `git diff --check`, no-LiveCoding cold compile, and `[TDD] UnrealBridge_StartPIE_PlayerStartPolicy` evidence.
