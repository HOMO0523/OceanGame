"""
Maintain a lightweight documentation-sync snapshot for AI production work.

This script is intentionally non-blocking and is run only as a manual or
pre-commit checkpoint, not after every edit. With --apply-memory it updates the
bounded block in memory-bank/progress.md so project memory follows the
video-style production structure at commit boundaries.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parent
REPORT_ROOT = PROJECT_ROOT / "Saved" / "HarnessReports"
LATEST_MD = REPORT_ROOT / "doc-sync-latest.md"
LATEST_JSON = REPORT_ROOT / "doc-sync-latest.json"
PROGRESS_PATH = PROJECT_ROOT / "memory-bank" / "progress.md"

START_MARKER = "<!-- DOC_SYNC_HOOK:START -->"
END_MARKER = "<!-- DOC_SYNC_HOOK:END -->"


@dataclass
class ChangedFile:
    status: str
    path: str
    targets: set[str] = field(default_factory=set)
    stages: set[str] = field(default_factory=set)


def run_command(command: list[str]) -> tuple[int, str, str]:
    result = subprocess.run(command, cwd=PROJECT_ROOT, capture_output=True, text=True)
    return result.returncode, result.stdout, result.stderr


def read_stdin_json() -> dict[str, Any]:
    try:
        raw = sys.stdin.read()
    except Exception:
        return {}
    if not raw.strip():
        return {}
    try:
        return json.loads(raw)
    except json.JSONDecodeError:
        return {"raw": raw[:2000]}


def normalize_path(path: str) -> str:
    return path.replace("\\", "/").strip().strip('"')


def parse_git_status() -> list[ChangedFile]:
    code, stdout, _ = run_command(["git", "status", "--short"])
    if code != 0:
        return []

    changed: list[ChangedFile] = []
    for raw in stdout.splitlines():
        if len(raw) < 4:
            continue
        status = raw[:2].strip() or raw[:2]
        path = normalize_path(raw[3:])
        if " -> " in path:
            path = path.split(" -> ", 1)[1]
        entry = ChangedFile(status=status, path=path)
        entry.targets, entry.stages = classify_path(path)
        changed.append(entry)
    return changed


def classify_path(path: str) -> tuple[set[str], set[str]]:
    p = normalize_path(path).lower()
    targets: set[str] = set()
    stages: set[str] = set()

    if p in {"agents.md", "claude.md"} or p.startswith(".claude/settings"):
        targets.update({"AGENTS.md", "CLAUDE.md"})
        stages.add("00 context and rules")

    if p.startswith("memory-bank/"):
        targets.add(path)
        stages.add("08 memory and registry update")

    if p.startswith("docs/production/"):
        targets.add("docs/production")
        stages.add("01 production unit split")
        if p.endswith("00-raw-input.md"):
            stages.add("00 raw input")
        elif p.endswith("01-semantics.md"):
            stages.add("02 semantic freeze")
        elif p.endswith("02-infra-audit.md"):
            stages.add("03 infrastructure audit")
        elif p.endswith("03-plan.md"):
            stages.add("04 implementation plan")
        elif p.endswith("04-tests.md"):
            stages.add("05 test design")
        elif p.endswith("05-implementation-log.md"):
            stages.add("06 implementation log")
        elif p.endswith("06-test-results.md"):
            stages.add("07 verification and repair")
        elif p.endswith("07-review.md"):
            stages.add("08 review")

    if p.startswith("docs/superpowers/specs/"):
        targets.add("docs/superpowers/specs")
        stages.update({"00 context and rules", "08 memory and registry update"})
    elif p.startswith("docs/superpowers/plans/"):
        targets.add("docs/superpowers/plans")
        stages.add("04 implementation plan")
    elif p.startswith("docs/design/"):
        targets.add("docs/design")
        stages.add("00 context and rules")

    if p.startswith("scripts/"):
        targets.update({"memory-bank/tech-stack.md", "memory-bank/progress.md"})
        stages.update({"03 infrastructure audit", "07 verification and repair"})

    if p.startswith("plugins/") or p.startswith("source/"):
        targets.update({"memory-bank/progress.md", "memory-bank/architecture.md"})
        stages.add("06 implementation log")
        if p.endswith((".build.cs", ".uplugin", ".uproject")):
            targets.add("memory-bank/tech-stack.md")
            stages.add("03 infrastructure audit")

    if p.startswith("config/") or p.endswith((".ini", ".target.cs")):
        targets.update({"memory-bank/architecture.md", "memory-bank/tech-stack.md"})
        stages.add("03 infrastructure audit")

    if p.startswith("content/"):
        targets.update({"memory-bank/progress.md", "memory-bank/architecture.md"})
        stages.add("06 implementation log")

    if p.startswith(".agents/skills/") or p.startswith(".claude/skills/"):
        targets.update({"AGENTS.md", "CLAUDE.md", "memory-bank/tech-stack.md", "memory-bank/progress.md"})
        stages.update({"00 context and rules", "03 infrastructure audit"})

    return targets, stages


def validator_report() -> dict[str, Any]:
    code, stdout, stderr = run_command([sys.executable, "scripts/harness_state_validator.py", "--json"])
    try:
        parsed = json.loads(stdout)
    except json.JSONDecodeError:
        parsed = {
            "success": False,
            "error_count": 1,
            "warning_count": 0,
            "findings": [{"severity": "error", "path": "scripts/harness_state_validator.py", "message": stderr.strip() or stdout[:500]}],
            "units": [],
        }
    parsed["exit_code"] = code
    return parsed


def changed_path_set(changed: list[ChangedFile]) -> set[str]:
    return {entry.path.lower() for entry in changed}


def derive_actions(changed: list[ChangedFile], validation: dict[str, Any]) -> list[str]:
    paths = changed_path_set(changed)
    actions: list[str] = []

    code_changed = any(path.startswith(("plugins/", "source/", "config/", "content/")) for path in paths)
    cpp_changed = any(path.endswith((".cpp", ".h", ".hpp", ".inl")) and path.startswith(("plugins/", "source/")) for path in paths)
    docs_changed = any(path.startswith(("memory-bank/", "docs/production/")) for path in paths)
    production_doc_changed = any(path.startswith("docs/production/") for path in paths)
    active_locks = {
        str(unit.get("parallel_lock", "")).strip()
        for unit in validation.get("units", [])
        if unit.get("active") and unit.get("parallel_lock")
    }
    architecture_changed = "memory-bank/architecture.md" in paths
    progress_changed = "memory-bank/progress.md" in paths
    tech_changed = "memory-bank/tech-stack.md" in paths

    if code_changed and not progress_changed:
        actions.append("Update `memory-bank/progress.md` with implementation/test status before handoff.")
    if any(path.endswith((".build.cs", ".uplugin", ".uproject", ".ini")) or path.startswith("scripts/") for path in paths) and not tech_changed:
        actions.append("Check whether `memory-bank/tech-stack.md` needs tooling/dependency updates.")
    if any(path.startswith(("plugins/", "source/", "config/", "content/")) for path in paths) and not architecture_changed:
        actions.append("Check whether `memory-bank/architecture.md` needs subsystem/data-flow updates.")
    if code_changed and not production_doc_changed:
        actions.append("Tie code changes to an active `docs/production/*` unit or create one.")
    if code_changed and production_doc_changed and len(active_locks) > 1:
        actions.append("Confirm changed code belongs to exactly one active `parallel_lock`; multiple active locks require coordinator routing.")
    if cpp_changed:
        actions.append("For UE C++ changes, confirm `[TDD]` logs were added before implementation and run the UE TDD pipeline.")
    if not validation.get("success", False):
        actions.append("Fix `scripts/harness_state_validator.py --json` findings before parallel work.")
    if docs_changed and validation.get("success", False):
        actions.append("Production docs validate; keep `07-review.md` decision aligned with actual test evidence.")
    if not actions:
        actions.append("No immediate documentation drift detected by the hook.")

    return actions


def video_stage_table(changed: list[ChangedFile], validation: dict[str, Any]) -> list[dict[str, str]]:
    touched = set().union(*(entry.stages for entry in changed)) if changed else set()
    units = validation.get("units", [])
    active_units = [unit for unit in units if unit.get("active")]
    rows = [
        ("00 context and rules", "AGENTS/memory-bank/spec authority is available", "covered"),
        ("01 production unit split", f"{len(units)} production unit(s), {len(active_units)} active", "covered" if units else "gap"),
        ("02 semantic freeze", "01-semantics.md exists per unit", "covered" if units else "gap"),
        ("03 infrastructure audit", "02-infra-audit.md plus tool registry/static checks", "covered" if units else "gap"),
        ("04 implementation plan", "03-plan.md is required by validator", "covered" if units else "gap"),
        ("05 test design", "04-tests.md is required by validator", "covered" if units else "gap"),
        ("06 implementation log", "05-implementation-log.md is required by validator", "covered" if units else "gap"),
        ("07 verification and repair", "06-test-results.md plus failure classification", "covered" if units else "gap"),
        ("08 review", "07-review.md decision gates terminal state", "covered" if units else "gap"),
        ("09 memory and registry update", "hook snapshot can update memory-bank/progress.md during explicit/pre-commit sync", "covered"),
    ]
    table: list[dict[str, str]] = []
    for stage, evidence, status in rows:
        table.append({
            "stage": stage,
            "status": "touched" if stage in touched else status,
            "evidence": evidence,
        })
    return table


def build_report(phase: str, hook_input: dict[str, Any]) -> dict[str, Any]:
    changed = parse_git_status()
    validation = validator_report()
    all_targets = sorted(set().union(*(entry.targets for entry in changed)) if changed else set())
    report = {
        "generated_at": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "phase": phase,
        "hook_event_name": hook_input.get("hook_event_name", phase),
        "tool_name": hook_input.get("tool_name", ""),
        "changed_files": [
            {
                "status": entry.status,
                "path": entry.path,
                "doc_targets": sorted(entry.targets),
                "video_stages": sorted(entry.stages),
            }
            for entry in changed
        ],
        "doc_targets": all_targets,
        "validator": validation,
        "video_stages": video_stage_table(changed, validation),
    }
    report["actions"] = derive_actions(changed, validation)
    return report


def render_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Doc Sync Hook Report",
        "",
        f"- generated_at: {report['generated_at']}",
        f"- phase: `{report['phase']}`",
        f"- hook_event_name: `{report['hook_event_name']}`",
        f"- tool_name: `{report['tool_name']}`",
        "",
        "## Video Flow Structure",
        "",
        "| Stage | Status | Evidence |",
        "|---|---|---|",
    ]
    for row in report["video_stages"]:
        lines.append(f"| {row['stage']} | {row['status']} | {row['evidence']} |")

    lines.extend(["", "## Documentation Targets", ""])
    if report["doc_targets"]:
        for target in report["doc_targets"]:
            lines.append(f"- `{target}`")
    else:
        lines.append("- No changed-file target detected.")

    lines.extend(["", "## Active Production Units", ""])
    units = report["validator"].get("units", [])
    active_units = [unit for unit in units if unit.get("active")]
    if active_units:
        for unit in active_units:
            name = Path(unit.get("unit_dir", "")).name
            lines.append(f"- `{name}` lock=`{unit.get('parallel_lock', '')}` findings={unit.get('finding_count', 0)}")
    else:
        lines.append("- No active production unit.")

    lines.extend(["", "## Suggested Actions", ""])
    for action in report["actions"]:
        lines.append(f"- {action}")

    lines.extend(["", "## Changed Files", ""])
    if report["changed_files"]:
        lines.extend(["| Status | File | Doc Targets |", "|---|---|---|"])
        for entry in report["changed_files"][:80]:
            targets = ", ".join(f"`{target}`" for target in entry["doc_targets"]) or "-"
            lines.append(f"| `{entry['status']}` | `{entry['path']}` | {targets} |")
        if len(report["changed_files"]) > 80:
            lines.append(f"| ... | {len(report['changed_files']) - 80} more | ... |")
    else:
        lines.append("- Working tree has no changed files.")

    validation = report["validator"]
    lines.extend([
        "",
        "## Harness Validator",
        "",
        f"- success: `{validation.get('success')}`",
        f"- unit_count: `{validation.get('unit_count', 0)}`",
        f"- error_count: `{validation.get('error_count', 0)}`",
        f"- warning_count: `{validation.get('warning_count', 0)}`",
    ])
    for finding in validation.get("findings", []):
        lines.append(f"- {finding.get('severity', 'finding')}: `{finding.get('path', '')}` {finding.get('message', '')}")
    lines.append("")
    return "\n".join(lines)


def write_reports(report: dict[str, Any], history: bool) -> Path:
    REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    markdown = render_markdown(report)
    LATEST_MD.write_text(markdown, encoding="utf-8")
    LATEST_JSON.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
    if history:
        path = REPORT_ROOT / f"{time.strftime('%Y%m%d-%H%M%S')}-doc-sync.md"
        path.write_text(markdown, encoding="utf-8")
        return path
    return LATEST_MD


def build_memory_block(report: dict[str, Any], report_path: Path) -> str:
    active_units = [
        Path(unit.get("unit_dir", "")).name
        for unit in report["validator"].get("units", [])
        if unit.get("active")
    ]
    target_text = ", ".join(f"`{target}`" for target in report["doc_targets"][:8]) or "no changed-file target"
    action_text = "\n".join(f"- {action}" for action in report["actions"][:6])
    active_text = ", ".join(f"`{unit}`" for unit in active_units) or "none"
    relative_report = report_path.relative_to(PROJECT_ROOT).as_posix() if report_path.is_relative_to(PROJECT_ROOT) else report_path.as_posix()

    stage_rows = "\n".join(
        f"- {row['stage']}: {row['status']}"
        for row in report["video_stages"]
    )

    return "\n".join([
        START_MARKER,
        "### Doc Sync Hook Snapshot",
        "",
        f"- generated_at: {report['generated_at']}",
        f"- phase: `{report['phase']}`",
        f"- latest_report: `{{ProjectRoot}}/{relative_report}`",
        f"- active_units: {active_text}",
        f"- doc_targets: {target_text}",
        f"- validator: success=`{report['validator'].get('success')}` errors=`{report['validator'].get('error_count', 0)}` warnings=`{report['validator'].get('warning_count', 0)}`",
        "",
        "**Video flow status:**",
        stage_rows,
        "",
        "**Next documentation actions:**",
        action_text,
        END_MARKER,
    ])


def update_progress_snapshot(report: dict[str, Any], report_path: Path) -> bool:
    block = build_memory_block(report, report_path)
    text = PROGRESS_PATH.read_text(encoding="utf-8", errors="replace")
    if START_MARKER in text and END_MARKER in text:
        start = text.index(START_MARKER)
        end = text.index(END_MARKER) + len(END_MARKER)
        new_text = text[:start] + block + text[end:]
    else:
        new_text = text.rstrip() + "\n\n## Hook Maintained Project Doc Sync\n\n" + block + "\n"
    if new_text != text:
        try:
            PROGRESS_PATH.write_text(new_text, encoding="utf-8")
        except PermissionError:
            return False
        return True
    return False


def main() -> int:
    parser = argparse.ArgumentParser(description="Update Ocean documentation-sync hook reports")
    parser.add_argument("--phase", default="manual", help="Hook phase, e.g. manual or pre-commit")
    parser.add_argument("--apply-memory", action="store_true", help="Update the bounded progress.md hook snapshot")
    parser.add_argument("--history", action="store_true", help="Write a timestamped report in addition to latest files")
    parser.add_argument("--quiet", action="store_true", help="Suppress normal stdout")
    args = parser.parse_args()

    hook_input = read_stdin_json()
    report = build_report(args.phase, hook_input)
    report_path = write_reports(report, history=args.history)
    memory_updated = update_progress_snapshot(report, report_path) if args.apply_memory else False

    if not args.quiet:
        print(json.dumps({
            "continue": True,
            "doc_sync_report": str(report_path),
            "memory_updated": memory_updated,
            "validator_success": report["validator"].get("success", False),
            "active_units": [
                Path(unit.get("unit_dir", "")).name
                for unit in report["validator"].get("units", [])
                if unit.get("active")
            ],
        }, ensure_ascii=False))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
