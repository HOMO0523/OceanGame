---
unit_id: 2026-06-16-automation-migration
status: frozen
owner: clarifier
updated_at: 2026-06-16T16:35:00
source_commit: working-tree
depends_on: []
parallel_lock: Ocean.EditorAutomation
---

# Semantics

unit_type: infra
name: Ocean automation migration
capability: Project-local UnrealBridge, no-LiveCoding TDD pipeline, production-unit validation, document drift checks, and reusable agent skills.
owned_contract: Ocean agents can automate editor work, save packages, run PIE/log capture, and follow repeatable production-unit workflows from project files alone.
callers: Codex agents, local scripts, future subagents, and human maintainers.
forbidden_dependencies: Roguelike gameplay plugins, BallGameChaos runtime modules, BallGame-specific UI/economy/balance scripts, generated plugin binaries.
accepted_target: Ocean-specific automation files exist and refer to Ocean project names, targets, and Water/build-grid/PCG/buoyancy acceptance.
forbidden_fallbacks: Manual editor clicks as first choice, Live Coding as compile proof, killing an editor with possible unsaved work, copying source project gameplay assumptions.
inputs: Current Ocean repository, reference automation project, UE 5.7 install.
outputs: `Plugins/UnrealBridge`, `scripts/*`, `AGENTS.md`, `.agents/skills/*`, `docs/design/agent-operating-guide.md`, `memory-bank/*`.
phase_or_timing_rules: Use bridge save gate before editor shutdown; use `--check-only` only for log inspection.
edge_cases: Missing bridge blocks editor shutdown; no production units is acceptable but reported as a warning; running editor may block `OceanEditor` build.
visual_acceptance: Not visual; accepted through file presence, script syntax, validator output, and build feasibility.
automation_probe: `python -m py_compile ...`, `python scripts/harness_state_validator.py --json`, `python scripts/doc_sync_hook.py --phase manual --history`, `git diff --check`.
tests_required: Python syntax, production validator, doc-sync hook, residual source-project name scan, game-target build, editor-target blocker classification.
remaining_questions: []
