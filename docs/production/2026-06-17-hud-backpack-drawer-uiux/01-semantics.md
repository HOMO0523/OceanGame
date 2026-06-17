---
unit_id: 2026-06-17-hud-backpack-drawer-uiux
status: approved
owner: clarifier
updated_at: 2026-06-17T16:10:00+08:00
source_commit: 76ddd07
depends_on: [2026-06-16-minimal-loop-workflow]
parallel_lock: Ocean.UI.InventoryPlacement
---

# Semantics

unit_type: ui
name: Ocean HUD and right-side backpack drawer
accepted_target: A low-obstruction survival HUD with a right-side backpack drawer, item slot model, use-item command path, and placement-preview command path.
forbidden_fallbacks: Full-screen inventory, UI-only fake placement success, direct UMG actor spawning, replacing `B` build toggle with backpack, removing existing debug/build systems before the WBP replacement is proven.
owned_contract: Ocean UI owns presentation, input-mode changes, widget visibility, and dispatching user intent; gameplay components own inventory mutation, stat recovery, build validation, and actor spawning.
callers: `AOceanPlayerController`, `BP_OceanMVPPlayerController`, `BP_OceanSurvivorCharacter`, `UOceanInventoryComponent`, `UOceanSurvivalComponent`, `UOceanBuildComponent`.
inputs: `Tab/I`, backpack button click, slot click, slot drag, confirm modal choice, right-click, `Esc`, `R`, left click during placement preview.
outputs: Visible HUD, drawer open/closed state, item slot view data, use-item requests, drag/drop results, placement query results, toast/error feedback.
phase_or_timing_rules: Closed HUD is visible during normal play; backpack drawer uses `Game and UI`; modal state pauses lower-priority commands; placement preview takes priority over left-click movement.
edge_cases: Full inventory, invalid slot index, item disappearing during drag, key item discard, status already full, insufficient resources, invalid world hit, occupied grid cell, detached placement, wrong zone, destroyed platform.
visual_acceptance: `L_WaterOcean` PIE shows closed HUD by default, right-side drawer after backpack input, status/time/top-right panels in their sketch-aligned locations, and placement overlay text when a placeable item enters preview.
automation_probe: UnrealBridge script loads `/Game/OceanPrototype/Maps/L_WaterOcean`, starts PIE, verifies `WBP_OceanHUDRoot` is created, toggles backpack state, captures `[TDD] OceanUI*` log lines, and confirms the widget assets exist.
tests_required: `Ocean.MVP.Inventory.UseItem`, `Ocean.MVP.Inventory.DragDropModel`, `Ocean.MVP.Build.PlacementQuery`, `Ocean.MVP.UI.HUDStateModel`, `BridgeVerifyOceanUIAssets`, `BridgeVerifyOceanHUDPIE`.
remaining_questions: []

screen_set: Closed survival HUD, right-side backpack drawer, confirm modal, placement overlay.
accepted_asset_paths: `/Game/OceanPrototype/UI/WBP_OceanHUDRoot`, `/Game/OceanPrototype/UI/WBP_StatusPanel`, `/Game/OceanPrototype/UI/WBP_TimePanel`, `/Game/OceanPrototype/UI/WBP_TopRightPanel`, `/Game/OceanPrototype/UI/Inventory/WBP_BackpackDrawer`, `/Game/OceanPrototype/UI/Inventory/WBP_InventorySlot`, `/Game/OceanPrototype/UI/Inventory/WBP_ItemDragVisual`, `/Game/OceanPrototype/UI/Build/WBP_PlacementOverlay`, `/Game/OceanPrototype/UI/Common/WBP_ConfirmModal`, `/Game/OceanPrototype/UI/Common/WBP_ToastStack`.
required_controls: Stamina/hydration/satiety bars, day/time/event text, backpack open button, inventory grid slots, item detail panel, confirm/cancel modal buttons, placement validity text, rotate/cancel hints.
data_bindings: Survival stats from `UOceanSurvivalComponent`, inventory slots from `UOceanInventoryComponent`, placement validity from `UOceanBuildComponent` and `UOceanBuildGridComponent`.
command_dispatch: UI dispatches use-item, move-slot, begin-placement-preview, confirm-placement, cancel-placement, and close-drawer commands.
fallback_policy: Widget art may use colored boxes and text; missing icons use placeholder brush colors; missing models use cube placeholders only through gameplay actors that preserve buoyancy/component contracts.
