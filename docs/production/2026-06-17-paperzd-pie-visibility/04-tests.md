---
unit_id: 2026-06-17-paperzd-pie-visibility
status: approved
owner: debugger
updated_at: 2026-06-17T12:55:00+08:00
source_commit: 2c9e304
depends_on: [2026-06-17-paper2d-state-machine]
parallel_lock: Ocean.Paper2DVisibility
---

# Tests

- T-001: `[TDD] UnrealBridge_StartPIE_PlayerStartPolicy` logs `actual=1 expected=1` when the editor world contains at least one `APlayerStart` and Bridge leaves `StartLocation` unset.
- T-002: no-LiveCoding pipeline compiles and runs PIE after PaperZD is enabled.
- T-003: runtime probe confirms PaperZD plugin is loaded, Survivor pawn is possessed, and Paper2D visual has a valid Flipbook.
- T-004: screenshot evidence is captured under `Saved/Diagnostics` for the pre-fix invisible symptom and post-fix visibility check.
