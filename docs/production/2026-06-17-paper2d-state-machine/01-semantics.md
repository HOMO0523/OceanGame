---
unit_id: 2026-06-17-paper2d-state-machine
status: frozen
owner: clarifier
updated_at: 2026-06-17T16:55:00+08:00
source_commit: 2341511
depends_on: [2026-06-17-paper2d-animation-set]
parallel_lock: Ocean.Paper2DStateMachine
---

# Semantics

unit_type: gameplay_slice
name: Paper2D camera-facing animation state machine
playable_loop: player moves the existing Ocean survivor while the Paper2D visual faces the camera and switches idle/walk/jump Flipbooks
required_systems: `AOceanCharacter`, `UPaperFlipbookComponent`, new Paper2D animation component, imported V5WalkSafe Flipbooks, top-down camera
phase_flow: Tick reads movement and camera state, chooses visual state and cardinal direction, then updates Paper2D component rotation and Flipbook
player_actions: left-click move, WASD move, Space jump, future event requests for swim/climb/divesuit visual states
feedback_moments: sprite visibly remains camera-facing; moving changes from idle to walk; jumping changes from walk/idle to jump
upgrade_rule: future event systems may request swim, climb, or dive-suit states without replacing the gameplay pawn
failure_cases: no camera available, missing Flipbook assignment, zero velocity, jump while moving, imported assets missing, gameplay actor rotation accidentally changed
visual_readability: Paper2D character should read as a camera-facing 2D sprite over the 3D ocean/platform scene from the fixed top-down camera
accepted_target: the survivor keeps the Paper2D visual facing the active camera and switches among imported Flipbooks based on lightweight visual state and movement direction
forbidden_fallbacks: rotating `AOceanCharacter` actor just to face camera; replacing `BP_OceanSurvivorCharacter`; treating one default Flipbook as finished animation; adding full diving/fishing/island gameplay
owned_contract: `UOceanPaper2DAnimationComponent` owns visual state selection and camera-facing presentation; `UPaperFlipbookComponent` owns sprite playback only; `AOceanCharacter` remains gameplay pawn
callers: `AOceanCharacter::Tick`; future event systems may request `Swim`, `Climb`, or `DiveSuitDive`
inputs: character velocity, character movement falling state, active camera location, imported Paper2D Flipbook references
outputs: Paper2D visual component world rotation faces camera; current Paper2D Flipbook matches state and direction
phase_or_timing_rules: update every character Tick after movement has updated; never modify logical build grid or actor gameplay rotation for visual billboard
edge_cases: zero velocity keeps last direction for idle; missing Flipbook keeps current valid Flipbook; no active camera skips facing update without breaking movement
visual_acceptance: `BP_OceanSurvivorCharacter` has Paper2D visual and animation components; imported V5WalkSafe Flipbooks are assigned to all six states and four directions; camera-facing affects only the Paper2D component
automation_probe: C++ automation tests under `Ocean.Paper2D.Animation`; Bridge verifier for component presence and 24 assigned Flipbooks
tests_required: direction mapping chooses north/east/south/west from velocity; state priority chooses jump over walk; camera-facing yaw points Paper2D visual toward camera; BP defaults contain 24 Flipbook assignments
remaining_questions: []
