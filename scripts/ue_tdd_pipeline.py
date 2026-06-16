"""
Ocean UE TDD No-LiveCoding Pipeline — automated save → close → build → run → test → analyze cycle.

Usage:
    python scripts/ue_tdd_pipeline.py                    # Full cycle
    python scripts/ue_tdd_pipeline.py --no-build          # Skip build (editor already running; not compile proof)
    python scripts/ue_tdd_pipeline.py --no-launch         # Build only; skip launch, PIE, and log capture
    python scripts/ue_tdd_pipeline.py --pie-duration 10   # Run PIE for 10 seconds
    python scripts/ue_tdd_pipeline.py --check-only        # Only capture/analyze logs (no build/launch)
"""

import subprocess
import sys
import time
import argparse
from pathlib import Path

# === Config ===
# Auto-detect project root from script location
_SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = _SCRIPT_DIR.parent
UPROJECT = PROJECT_ROOT / "Ocean.uproject"

# UE 5.7 path — check common locations
_UE_CANDIDATES = [
    Path(r"E:\epic\UE_5.7"),
    Path(r"E:\UE_5.7"),
    Path(r"D:\UE_5.7"),
    Path(r"C:\Program Files\Epic Games\UE_5.7"),
]
UE_ROOT = None
for _cand in _UE_CANDIDATES:
    _ue_editor = _cand / "Engine" / "Binaries" / "Win64" / "UnrealEditor.exe"
    if _ue_editor.exists():
        UE_ROOT = _cand
        break

if UE_ROOT is None:
    # Fallback: try to find via registry or parent of UE_5.5
    import os as _os
    _ue55 = Path(r"E:\UE_5.5")
    if _ue55.exists():
        UE_ROOT = _ue55  # fallback to 5.5
    else:
        raise RuntimeError(
            "Cannot find Unreal Engine installation. "
            "Checked: " + ", ".join(str(c) for c in _UE_CANDIDATES)
        )

UE_EDITOR = UE_ROOT / "Engine" / "Binaries" / "Win64" / "UnrealEditor.exe"
UE_BUILD_BAT = UE_ROOT / "Engine" / "Build" / "BatchFiles" / "Build.bat"
BUILD_TARGET = "OceanEditor"
BUILD_CONFIG = "Development"
SCRIPT_DIR = Path(__file__).resolve().parent
BRIDGE_CLIENT = SCRIPT_DIR / "ue_tdd_bridge.py"

sys.path.insert(0, str(SCRIPT_DIR))
from ue_tdd_bridge import BridgeClient


# === Editor Lifecycle ===

def kill_editor() -> bool:
    """Kill any running UnrealEditor process. Returns True if a process was killed."""
    result = subprocess.run(
        ["taskkill", "/f", "/im", "UnrealEditor.exe"],
        capture_output=True, text=True
    )
    killed = "SUCCESS" in result.stdout.upper() or result.returncode == 0
    if killed:
        print("[KILL] UnrealEditor terminated")
    else:
        print("[KILL] No running editor found")
    return killed


def is_editor_running() -> bool | None:
    """Return whether UnrealEditor is running, or None if process query is unavailable."""
    result = subprocess.run(
        ["tasklist", "/FI", "IMAGENAME eq UnrealEditor.exe"],
        capture_output=True, text=True
    )
    if result.returncode != 0:
        ps_result = subprocess.run(
            [
                "powershell",
                "-NoProfile",
                "-Command",
                "if (Get-Process -Name UnrealEditor -ErrorAction SilentlyContinue) { 'RUNNING' } else { 'NOT_RUNNING' }",
            ],
            capture_output=True, text=True
        )
        if ps_result.returncode != 0:
            return None
        ps_output = (ps_result.stdout or "") + (ps_result.stderr or "")
        if "NOT_RUNNING" in ps_output:
            return False
        if "RUNNING" in ps_output:
            return True
        return None
    output = (result.stdout or "") + (result.stderr or "")
    return "UnrealEditor.exe" in output


def save_running_editor_before_close() -> bool:
    """Save dirty editor packages through UnrealBridge before closing the editor."""
    running = is_editor_running()
    if running is False:
        print("[SAVE] No running editor detected")
        return True

    print("[SAVE] Editor may be running; attempting UnrealBridge save before close...")
    client = BridgeClient()
    if not client.connect():
        if running is True:
            print("[SAVE] FAILED: editor is running but bridge is unavailable; refusing to close before compile")
            return False
        print("[SAVE] Bridge unavailable and editor state unknown; refusing to close before compile")
        return False

    script = r"""
import unreal
dirty_before = [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()]
dirty_before += [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()]
save_result = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
dirty_after = [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()]
dirty_after += [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()]
print(f"save_result={save_result} dirty_before={dirty_before} dirty_after={dirty_after}")
"""
    response = client.send(script, timeout=120)
    output = response.get("output", "").strip()
    if output:
        print(f"[SAVE] {output}")
    if not response.get("success") or "save_result=True" not in output:
        print("[SAVE] FAILED: dirty package save did not report success; refusing to close before compile")
        return False
    return True


def build_project() -> bool:
    """Run UnrealBuildTool. Returns True on success."""
    cmd = [
        str(UE_BUILD_BAT),
        BUILD_TARGET, "Win64", BUILD_CONFIG,
        f'-Project={UPROJECT.as_posix()}',
        "-NoHotReload",
    ]
    print(f"[BUILD] {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=str(UE_BUILD_BAT.parent.parent.parent))
    success = result.returncode == 0
    if success:
        print("[BUILD] Compile succeeded")
    else:
        print(f"[BUILD] Compile FAILED (exit {result.returncode})")
    return success


def launch_editor() -> subprocess.Popen | None:
    """Launch UE editor in background. Returns the Popen handle."""
    print(f"[LAUNCH] Starting editor...")
    try:
        proc = subprocess.Popen(
            [str(UE_EDITOR), UPROJECT.as_posix(), "-UnrealBridgeForceReady"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        print(f"[LAUNCH] Editor PID: {proc.pid}")
        return proc
    except Exception as e:
        print(f"[LAUNCH] Failed: {e}")
        return None


def wait_for_bridge(timeout: float = 120.0, interval: float = 3.0) -> BridgeClient | None:
    """Wait for UnrealBridge to become ready. Returns connected BridgeClient or None."""
    print(f"[WAIT] Waiting for bridge (timeout={timeout}s)...")
    deadline = time.time() + timeout
    client = BridgeClient()

    while time.time() < deadline:
        if client.connect():
            try:
                r = client.send("print('ok')")
                if r.get("success"):
                    print(f"[WAIT] Bridge ready on port {client.port}")
                    return client
            except Exception:
                pass
        print(f"  ... waiting ({interval}s)")
        time.sleep(interval)

    print("[WAIT] Bridge connection timed out")
    return None


# === TDD Pipeline ===

def run_tdd_cycle(
    build: bool = True,
    launch: bool = True,
    pie_duration: float = 5.0,
    log_lines: int = 300,
    tdd_filter: str = "[TDD]",
    pre_pie_console_commands: list[str] | None = None,
) -> dict:
    """Run the full TDD cycle.

    Returns:
        dict with keys: success, tdd_lines, all_lines, pie_started, pie_duration
    """
    result = {"success": False, "tdd_lines": [], "all_lines": [], "pie_started": False}

    # Phase 4.1: Save and close editor before cold compile/launch
    if build or launch:
        if not save_running_editor_before_close():
            result["error"] = "Could not save running editor before close"
            return result
        kill_editor()
        time.sleep(2)

    # Phase 4.2: Build
    if build:
        if not build_project():
            result["error"] = "Build failed"
            return result

    if not launch:
        result["success"] = True
        return result

    # Phase 4.3: Launch editor
    launch_editor()

    # Phase 4.4: Wait for bridge
    client = wait_for_bridge()
    if client is None:
        result["error"] = "Bridge never became ready"
        return result

    # Give the editor a moment to fully settle
    time.sleep(3)

    # Phase 4.5: Clear logs + start PIE
    print("[TDD] Clearing log buffer and starting PIE...")
    client.clear_log_buffer()
    client.write_log(f"[TDD] Pipeline cycle start at {time.strftime('%H:%M:%S')}", "Log")

    if client.is_in_pie():
        print("[TDD] PIE was already running, stopping first...")
        client.stop_pie()
        time.sleep(2)

    for command in pre_pie_console_commands or []:
        print(f"[TDD] Executing pre-PIE console command: {command}")
        client.execute_console_command(command)
        time.sleep(0.5)

    if not client.start_pie():
        result["error"] = "Failed to start PIE"
        return result

    result["pie_started"] = True
    print(f"[TDD] PIE started, running for {pie_duration}s...")

    # Phase 4.6: Wait for simulation
    time.sleep(pie_duration)

    # Phase 4.7: Verify PIE is still running + capture logs
    world_time = client.get_pie_world_time()
    print(f"[TDD] PIE world time: {world_time:.2f}s")

    tdd_lines = client.filter_tdd_lines(num_lines=log_lines)
    all_logs_for_report = client.get_recent_log_lines(log_lines)
    all_lines = client.get_recent_log_lines(num_lines=log_lines)

    # Phase 4.8: Stop PIE
    client.stop_pie()
    print("[TDD] PIE stopped")

    # Additional wait for any post-PIE log flush
    time.sleep(1)
    tdd_lines2 = client.filter_tdd_lines(num_lines=log_lines)
    all_lines2 = client.get_recent_log_lines(num_lines=log_lines)

    # Merge
    result["tdd_lines"] = list(dict.fromkeys(tdd_lines + tdd_lines2))
    result["all_lines"] = all_lines + all_lines2
    result["success"] = True
    result["pie_duration"] = world_time

    return result


def analyze_logs(tdd_lines: list[str]) -> dict:
    """Analyze captured [TDD] log lines and produce a report.

    Expected format: [TDD] CheckName: actual=%d expected=%d
    Also supports: [TDD] CheckName: <any descriptive assertion>
    """
    report = {"total": len(tdd_lines), "passed": 0, "failed": 0, "details": []}

    for line in tdd_lines:
        # Try to parse structured assertion: actual=%d expected=%d
        detail = {"line": line, "status": "info"}
        b_structured_assertion = False

        if "actual=" in line.lower() and "expected=" in line.lower():
            import re
            actual_match = re.search(r"(?<![A-Za-z_])actual[=:]\s*([-\d.]+)", line, re.IGNORECASE)
            expected_match = re.search(r"(?<![A-Za-z_])expected[=:]\s*([-\d.]+)", line, re.IGNORECASE)
            if actual_match and expected_match:
                b_structured_assertion = True
                actual_val = actual_match.group(1)
                expected_val = expected_match.group(1)
                if actual_val == expected_val:
                    detail["status"] = "pass"
                    report["passed"] += 1
                else:
                    detail["status"] = "fail"
                    detail["actual"] = actual_val
                    detail["expected"] = expected_val
                    report["failed"] += 1
                detail["check"] = line.split("actual=")[0].strip()

        if not b_structured_assertion and ("PASS" in line.upper() or "OK" in line.upper()):
            detail["status"] = "pass"
            report["passed"] += 1
        elif not b_structured_assertion and ("FAIL" in line.upper() or "ERROR" in line.upper()):
            detail["status"] = "fail"
            report["failed"] += 1

        report["details"].append(detail)

    return report


# === CLI ===

def main():
    parser = argparse.ArgumentParser(description="UE TDD No-LiveCoding Pipeline")
    parser.add_argument("--no-build", action="store_true", help="Skip build step")
    parser.add_argument("--no-launch", action="store_true", help="Skip editor launch, bridge wait, PIE, and log capture after build")
    parser.add_argument("--check-only", action="store_true", help="Only capture and analyze logs")
    parser.add_argument("--pie-duration", type=float, default=5.0, help="Seconds to run PIE (default: 5)")
    parser.add_argument("--log-lines", type=int, default=300, help="Log lines to capture (default: 300)")
    parser.add_argument("--filter", type=str, default="[TDD]", help="Log filter pattern (default: [TDD])")
    parser.add_argument(
        "--pre-pie-console-command",
        action="append",
        default=[],
        help="Console command to execute before PIE; repeat for multiple commands",
    )
    args = parser.parse_args()

    print("=" * 60)
    print("  UE TDD No-LiveCoding Pipeline")
    print(f"  Project: {UPROJECT}")
    print("=" * 60)

    if args.check_only:
        print("[PIPELINE] Check-only mode — capturing logs from running editor")
        client = BridgeClient()
        if not client.connect():
            print("FATAL: Cannot connect to bridge. Is editor running?")
            sys.exit(1)
        tdd_lines = client.filter_tdd_lines(num_lines=args.log_lines)
        report = analyze_logs(tdd_lines)
    else:
        result = run_tdd_cycle(
            build=not args.no_build,
            launch=not args.no_launch,
            pie_duration=args.pie_duration,
            log_lines=args.log_lines,
            tdd_filter=args.filter,
            pre_pie_console_commands=args.pre_pie_console_command,
        )

        if not result.get("success"):
            print(f"\n[PIPELINE] FAILED: {result.get('error', 'Unknown error')}")
            sys.exit(1)

        if args.no_launch:
            print("\n[PIPELINE] Build-only mode complete; skipped editor launch, bridge wait, PIE, and log capture")
            sys.exit(0)

        print(f"\n[PIPELINE] Captured {len(result['all_lines'])} log lines total")
        report = analyze_logs(result["tdd_lines"])

    # Print report
    print("\n" + "=" * 60)
    print("  TDD Report")
    print("=" * 60)
    print(f"  Total [TDD] lines: {report['total']}")
    print(f"  Passed:            {report['passed']}")
    print(f"  Failed:            {report['failed']}")

    if report["details"]:
        print("\n  --- Details ---")
        for d in report["details"]:
            status_icon = "PASS" if d["status"] == "pass" else ("FAIL" if d["status"] == "fail" else "INFO")
            print(f"  [{status_icon}] {d['line'][:120]}")
            if d["status"] == "fail":
                print(f"           expected={d.get('expected', '?')}  actual={d.get('actual', '?')}")

    print("=" * 60)

    exit_code = 0 if report["failed"] == 0 else 1
    sys.exit(exit_code)


if __name__ == "__main__":
    main()
