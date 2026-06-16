"""
Validate AI Production Harness state folders.

The validator checks production-unit state files under docs/production:
- required 00..07 markdown files
- required metadata fields in every state file
- duplicate active parallel_lock values
- 01-semantics.md schema fields for known unit types

Usage:
    python scripts/harness_state_validator.py
    python scripts/harness_state_validator.py --require-units
    python scripts/harness_state_validator.py --root docs/production --json
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parent
DEFAULT_ROOT = PROJECT_ROOT / "docs" / "production"

REQUIRED_FILES = [
    "00-raw-input.md",
    "01-semantics.md",
    "02-infra-audit.md",
    "03-plan.md",
    "04-tests.md",
    "05-implementation-log.md",
    "06-test-results.md",
    "07-review.md",
]

REQUIRED_METADATA = [
    "unit_id",
    "status",
    "owner",
    "updated_at",
    "source_commit",
    "depends_on",
    "parallel_lock",
]

TERMINAL_STATUSES = {
    "ship",
    "shipped",
    "done",
    "complete",
    "completed",
    "closed",
}

UNIT_SCHEMAS: dict[str, list[str]] = {
    "building": [
        "name",
        "phase_available",
        "spawn_rule",
        "hit_trigger",
        "score_rule",
        "hp_rule",
        "cooldown_rule",
        "ball_color_interaction",
        "ball_type_interaction",
        "combo_hooks",
        "failure_cases",
        "visual_readability",
        "tests_required",
    ],
    "ability": [
        "name",
        "input_trigger",
        "valid_phases",
        "targeting_rule",
        "resource_cost",
        "cooldown_start_rule",
        "physics_intent",
        "effect_stack_rule",
        "pause_behavior",
        "failure_cases",
        "tests_required",
    ],
    "wave_goal": [
        "name",
        "start_condition",
        "success_condition",
        "failure_condition",
        "scoring_feedback",
        "timeout_behavior",
        "deterministic_seed_inputs",
        "tests_required",
    ],
    "autoui": [
        "name",
        "source_kind",
        "input_contract",
        "output_contract",
        "validation_rules",
        "asset_paths",
        "cache_key",
        "tests_required",
    ],
    "ui": [
        "name",
        "screen_set",
        "accepted_asset_paths",
        "required_controls",
        "data_bindings",
        "command_dispatch",
        "visual_acceptance",
        "automation_probe",
        "fallback_policy",
        "tests_required",
    ],
    "gameplay_slice": [
        "name",
        "playable_loop",
        "required_systems",
        "phase_flow",
        "player_actions",
        "feedback_moments",
        "upgrade_rule",
        "failure_cases",
        "visual_readability",
        "tests_required",
    ],
    "water_setup": [
        "name",
        "accepted_map_paths",
        "required_plugins",
        "collision_profile",
        "water_actors",
        "buoyancy_probe",
        "save_gate",
        "failure_cases",
        "tests_required",
    ],
    "build_grid": [
        "name",
        "cell_size",
        "module_footprint_rule",
        "rotation_rule",
        "overlap_rule",
        "adjacency_rule",
        "stable_grid_rule",
        "failure_cases",
        "tests_required",
    ],
    "build_module": [
        "name",
        "definition_asset_rule",
        "actor_class_rule",
        "placeholder_policy",
        "buoyancy_requirement",
        "resource_cost_rule",
        "placement_rule",
        "failure_cases",
        "tests_required",
    ],
    "resource_pcg": [
        "name",
        "resource_types",
        "pcg_graph_rule",
        "fallback_spawn_rule",
        "exclusion_rule",
        "collection_rule",
        "determinism_rule",
        "failure_cases",
        "tests_required",
    ],
    "command": [
        "name",
        "payload_type",
        "valid_phases",
        "state_transition",
        "side_effects",
        "rejection_cases",
        "commandlog_fields",
        "tests_required",
    ],
    "bugfix": [
        "name",
        "observed_failure",
        "expected_behavior",
        "reproduction_test",
        "failure_classification",
        "tests_required",
    ],
    "infra": [
        "name",
        "capability",
        "owned_contract",
        "callers",
        "forbidden_dependencies",
        "tests_required",
    ],
}


@dataclass
class Finding:
    severity: str
    path: str
    message: str


@dataclass
class UnitReport:
    unit_dir: Path
    metadata_by_file: dict[str, dict[str, str]] = field(default_factory=dict)
    active: bool = True
    parallel_lock: str = ""
    findings: list[Finding] = field(default_factory=list)


def normalize_status(status: str) -> str:
    return status.strip().lower().replace(" ", "-")


def add_finding(findings: list[Finding], severity: str, path: Path, message: str) -> None:
    findings.append(Finding(severity=severity, path=str(path), message=message))


def parse_metadata(path: Path) -> dict[str, str]:
    metadata: dict[str, str] = {}
    if not path.exists():
        return metadata

    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    in_front_matter = False
    saw_any = False

    for index, raw in enumerate(lines[:80]):
        line = raw.strip()
        if index == 0 and line == "---":
            in_front_matter = True
            continue
        if in_front_matter and line == "---":
            break
        if not line:
            if not in_front_matter and saw_any:
                break
            continue
        if line.startswith("#") and not in_front_matter:
            if saw_any:
                break
            continue
        match = re.match(r"^([A-Za-z0-9_-]+)\s*:\s*(.*)$", line)
        if not match:
            if in_front_matter:
                continue
            if saw_any:
                break
            continue
        key = match.group(1).strip().lower().replace("-", "_")
        value = match.group(2).strip().strip('"').strip("'")
        metadata[key] = value
        saw_any = True

    return metadata


def parse_schema_fields(path: Path) -> dict[str, str]:
    fields: dict[str, str] = {}
    if not path.exists():
        return fields

    in_code_block = False
    for raw in path.read_text(encoding="utf-8", errors="replace").splitlines():
        line = raw.rstrip()
        stripped = line.strip()
        if stripped.startswith("```"):
            in_code_block = not in_code_block
            continue
        if stripped.startswith("#") and not in_code_block:
            continue
        match = re.match(r"^\s*([A-Za-z0-9_-]+)\s*:\s*(.*)$", line)
        if match:
            key = match.group(1).strip().lower().replace("-", "_")
            value = match.group(2).strip().strip('"').strip("'")
            fields[key] = value
    return fields


def is_blank_schema_value(value: str) -> bool:
    return value.strip() in {"", "[]", "{}", "todo", "tbd", "null", "none", "-"}


def is_blank_metadata_value(field_name: str, value: str) -> bool:
    if field_name == "depends_on" and value.strip() == "[]":
        return False
    return is_blank_schema_value(value)


def validate_unit(unit_dir: Path) -> UnitReport:
    report = UnitReport(unit_dir=unit_dir)

    for filename in REQUIRED_FILES:
        path = unit_dir / filename
        if not path.exists():
            add_finding(report.findings, "error", path, "required state file is missing")
            continue

        metadata = parse_metadata(path)
        report.metadata_by_file[filename] = metadata
        for field_name in REQUIRED_METADATA:
            if is_blank_metadata_value(field_name, metadata.get(field_name, "")):
                add_finding(report.findings, "error", path, f"metadata field '{field_name}' is missing or blank")

    review_status = normalize_status(report.metadata_by_file.get("07-review.md", {}).get("status", ""))
    report.active = review_status not in TERMINAL_STATUSES

    lock_values = [
        meta.get("parallel_lock", "").strip()
        for meta in report.metadata_by_file.values()
        if meta.get("parallel_lock", "").strip()
    ]
    if lock_values:
        report.parallel_lock = lock_values[0]
        for lock in lock_values[1:]:
            if lock != report.parallel_lock:
                add_finding(
                    report.findings,
                    "error",
                    unit_dir,
                    f"inconsistent parallel_lock values: {sorted(set(lock_values))}",
                )

    semantics_path = unit_dir / "01-semantics.md"
    fields = parse_schema_fields(semantics_path)
    unit_type = fields.get("unit_type", "").strip().lower()
    if not semantics_path.exists():
        return report
    if not unit_type:
        add_finding(report.findings, "error", semantics_path, "unit_type is missing")
        return report
    if unit_type not in UNIT_SCHEMAS:
        add_finding(report.findings, "warning", semantics_path, f"unknown unit_type '{unit_type}', schema validation skipped")
        return report

    for field_name in UNIT_SCHEMAS[unit_type]:
        if is_blank_schema_value(fields.get(field_name, "")):
            add_finding(report.findings, "error", semantics_path, f"schema field '{field_name}' is missing or blank")

    return report


def discover_units(root: Path) -> list[Path]:
    if not root.exists():
        return []
    return sorted(path for path in root.iterdir() if path.is_dir())


def build_report(root: Path, require_units: bool) -> dict:
    findings: list[Finding] = []
    units = discover_units(root)
    unit_reports = [validate_unit(unit) for unit in units]

    if not root.exists():
        add_finding(findings, "warning", root, "production root does not exist yet")
    elif require_units and not units:
        add_finding(findings, "error", root, "no production units found")
    elif not units:
        add_finding(findings, "warning", root, "no production units found")

    active_locks: dict[str, list[Path]] = {}
    for report in unit_reports:
        if report.active and report.parallel_lock:
            active_locks.setdefault(report.parallel_lock, []).append(report.unit_dir)

    for lock, unit_dirs in active_locks.items():
        if len(unit_dirs) > 1:
            joined = ", ".join(path.name for path in unit_dirs)
            add_finding(findings, "error", root, f"duplicate active parallel_lock '{lock}' in: {joined}")

    all_findings = findings[:]
    for report in unit_reports:
        all_findings.extend(report.findings)

    errors = [finding for finding in all_findings if finding.severity == "error"]
    warnings = [finding for finding in all_findings if finding.severity == "warning"]

    return {
        "root": str(root),
        "unit_count": len(units),
        "error_count": len(errors),
        "warning_count": len(warnings),
        "success": len(errors) == 0,
        "findings": [finding.__dict__ for finding in all_findings],
        "units": [
            {
                "unit_dir": str(report.unit_dir),
                "active": report.active,
                "parallel_lock": report.parallel_lock,
                "finding_count": len(report.findings),
            }
            for report in unit_reports
        ],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate AI Production Harness state folders")
    parser.add_argument("--root", type=Path, default=DEFAULT_ROOT, help="Production root directory")
    parser.add_argument("--require-units", action="store_true", help="Fail when no production units exist")
    parser.add_argument("--json", action="store_true", help="Print machine-readable JSON")
    args = parser.parse_args()

    root = args.root
    if not root.is_absolute():
        root = PROJECT_ROOT / root

    report = build_report(root=root, require_units=args.require_units)

    if args.json:
        print(json.dumps(report, indent=2, ensure_ascii=False))
    else:
        print("AI Production Harness State Validator")
        print(f"Root: {report['root']}")
        print(f"Units: {report['unit_count']}")
        print(f"Errors: {report['error_count']}  Warnings: {report['warning_count']}")
        for finding in report["findings"]:
            print(f"[{finding['severity'].upper()}] {finding['path']}: {finding['message']}")

    return 0 if report["success"] else 1


if __name__ == "__main__":
    sys.exit(main())
