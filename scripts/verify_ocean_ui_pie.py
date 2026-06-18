"""External Bridge-driven PIE probe for the Ocean HUD root.

This script runs outside Unreal Editor. It loads L_WaterOcean through
UnrealBridge, starts PIE, waits on the client side so the editor can tick,
then verifies the real `[TDD] OceanHUDRootPIE: created=1` runtime log.
"""

from __future__ import annotations

import sys
import time
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))

from scripts.ue_tdd_bridge import BridgeClient


MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"
HUD_LOG_MARKER = "[TDD] OceanHUDRootPIE: created=1"


def tdd(name: str, message: str) -> None:
    print(f"[TDD] {name}: {message}")


def pass_fail(result: bool) -> str:
    return "PASS" if result else "FAIL"


def main() -> int:
    client = BridgeClient()
    if not client.connect():
        tdd("OceanUIPIEBridge", "result=FAIL error=connect")
        return 1

    if client.is_in_pie():
        client.stop_pie()
        time.sleep(1.0)

    load_script = f"""
import unreal
loaded = unreal.EditorLoadingAndSavingUtils.load_map('{MAP_PATH}')
print(bool(loaded))
"""
    load_response = client.send(load_script, timeout=120)
    loaded = load_response.get("success", False) and "True" in load_response.get("output", "")
    tdd("OceanUIPIEMapLoad", f"map={MAP_PATH} result={pass_fail(loaded)}")
    if not loaded:
        print(load_response.get("error", ""))
        print(load_response.get("output", ""))
        return 1

    client.clear_log_buffer()
    started = client.start_pie()
    tdd("OceanUIPIEStart", f"map={MAP_PATH} result={pass_fail(started)}")
    if not started:
        return 1

    time.sleep(3.0)
    world_time = client.get_pie_world_time()
    lines = client.get_recent_log_lines(num_lines=1200)
    client.stop_pie()

    found = any(HUD_LOG_MARKER in line for line in lines)
    tdd("OceanHUDRootPIE", f"real_created_log={1 if found else 0} world_time={world_time:.2f} result={pass_fail(found)}")
    if not found:
        interesting = [
            line
            for line in lines
            if "OceanHUDRootPIE" in line or "OceanPlayerController: BeginPlay" in line or "OceanMVPGameMode" in line
        ]
        print("\n".join(interesting[-40:]))
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
