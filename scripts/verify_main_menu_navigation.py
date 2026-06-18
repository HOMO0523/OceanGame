"""Bridge-driven verifier for the main menu New Game route.

The check runs against a real PIE session, inspects the runtime UMG button
delegate, then broadcasts the button click and confirms PIE travels to
L_WaterOcean.
"""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))

from scripts.ue_tdd_bridge import BridgeClient


MENU_MAP_PATH = "/Game/OceanPrototype/Maps/L_MainMenu"
TARGET_LEVEL_NAME = "L_WaterOcean"


def tdd(name: str, message: str) -> None:
    print(f"[TDD] {name}: {message}")


def pass_fail(result: bool) -> str:
    return "PASS" if result else "FAIL"


def bridge_print(client: BridgeClient, script: str, timeout: int = 30) -> str:
    response = client.send(script, timeout=timeout)
    output = response.get("output", "")
    if not response.get("success", False):
        error = response.get("error", "")
        if output:
            print(output)
        if error:
            print(error)
        raise RuntimeError(error or "UnrealBridge script failed")
    return output


def get_game_world_name(client: BridgeClient) -> str:
    script = """
import unreal
world = unreal.EditorLevelLibrary.get_game_world()
print(world.get_name() if world else "None")
"""
    return bridge_print(client, script).strip().splitlines()[-1]


def ensure_menu_pie(client: BridgeClient) -> None:
    if client.is_in_pie():
        world_name = get_game_world_name(client)
        if "L_MainMenu" in world_name:
            tdd("MainMenuPIEWorld", f"level={world_name} result=PASS")
            return
        client.stop_pie()
        time.sleep(1.0)

    load_script = f"""
import unreal
loaded = unreal.EditorLoadingAndSavingUtils.load_map('{MENU_MAP_PATH}')
print(bool(loaded))
"""
    output = bridge_print(client, load_script, timeout=120)
    loaded = "True" in output
    tdd("MainMenuMapLoad", f"map={MENU_MAP_PATH} result={pass_fail(loaded)}")
    if not loaded:
        raise RuntimeError(f"Could not load {MENU_MAP_PATH}")

    client.clear_log_buffer()
    started = client.start_pie()
    tdd("MainMenuPIEStart", f"map={MENU_MAP_PATH} result={pass_fail(started)}")
    if not started:
        raise RuntimeError("Could not start PIE")
    time.sleep(2.0)


def inspect_button_binding(client: BridgeClient) -> bool:
    script = r"""
import unreal

menu = None
button = None
for obj in unreal.ObjectIterator():
    try:
        class_name = obj.get_class().get_name()
        path = obj.get_path_name()
        if class_name == "OceanMainMenuWidget" and not obj.get_name().startswith("Default__") and "GameInstance" in path:
            menu = obj
        if class_name == "Button" and obj.get_name() == "NewGameButton" and "GameInstance" in path:
            button = obj
    except Exception:
        pass

has_menu = menu is not None
has_button = button is not None
is_bound = bool(button.on_clicked.is_bound()) if button else False
print(f"menu={1 if has_menu else 0} button={1 if has_button else 0} bound={1 if is_bound else 0}")
"""
    output = bridge_print(client, script)
    last_line = output.strip().splitlines()[-1] if output.strip() else ""
    parts = dict(
        item.split("=", 1)
        for item in last_line.split()
        if "=" in item
    )
    has_menu = parts.get("menu") == "1"
    has_button = parts.get("button") == "1"
    is_bound = parts.get("bound") == "1"
    tdd("MainMenuWidgetRuntime", f"menu={1 if has_menu else 0} button={1 if has_button else 0} result={pass_fail(has_menu and has_button)}")
    tdd("MainMenuNewGameButtonBound", f"actual={1 if is_bound else 0} expected=1 result={pass_fail(is_bound)}")
    return is_bound


def broadcast_new_game(client: BridgeClient) -> None:
    script = r"""
import unreal

button = None
for obj in unreal.ObjectIterator():
    try:
        if obj.get_class().get_name() == "Button" and obj.get_name() == "NewGameButton" and "GameInstance" in obj.get_path_name():
            button = obj
            break
    except Exception:
        pass

if not button:
    print("broadcast=0 reason=missing_button")
elif not button.on_clicked.is_bound():
    print("broadcast=0 reason=unbound")
else:
    button.on_clicked.broadcast()
    print("broadcast=1 reason=clicked")
"""
    output = bridge_print(client, script)
    last_line = output.strip().splitlines()[-1] if output.strip() else ""
    clicked = "broadcast=1" in last_line
    tdd("MainMenuNewGameBroadcast", f"{last_line} result={pass_fail(clicked)}")
    if not clicked:
        raise RuntimeError(last_line or "New Game broadcast failed")


def wait_for_target_level(client: BridgeClient, timeout_seconds: float = 8.0) -> bool:
    deadline = time.time() + timeout_seconds
    last_world = "None"
    while time.time() < deadline:
        last_world = get_game_world_name(client)
        if TARGET_LEVEL_NAME in last_world:
            tdd("MainMenuNewGameOpenLevel", f"level={last_world} expected_contains={TARGET_LEVEL_NAME} result=PASS")
            return True
        time.sleep(0.5)
    tdd("MainMenuNewGameOpenLevel", f"level={last_world} expected_contains={TARGET_LEVEL_NAME} result=FAIL")
    return False


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify main menu New Game opens WaterOcean in real PIE.")
    parser.add_argument("--inspect-only", action="store_true", help="Only verify the runtime button binding.")
    parser.add_argument("--leave-pie-running", action="store_true", help="Do not stop PIE after verification.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    client = BridgeClient()
    if not client.connect():
        tdd("MainMenuBridge", "result=FAIL error=connect")
        return 1

    try:
        ensure_menu_pie(client)
        is_bound = inspect_button_binding(client)
        if not is_bound:
            return 1
        if not args.inspect_only:
            broadcast_new_game(client)
            if not wait_for_target_level(client):
                return 1
        return 0
    finally:
        if not args.leave_pie_running and client.is_in_pie():
            client.stop_pie()


if __name__ == "__main__":
    raise SystemExit(main())
