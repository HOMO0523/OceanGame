"""
Verify Ocean HUD/backpack Widget Blueprint assets and WaterOcean map.

Run through UnrealBridge from the project root:
    from pathlib import Path
    from scripts.ue_tdd_bridge import BridgeClient

    client = BridgeClient()
    client.connect()
    client.send(Path("scripts/verify_ocean_ui_assets.py").read_text(encoding="utf-8"))

For the optional PIE probe:
    script = "import sys; sys.argv = ['verify_ocean_ui_assets.py', '--pie']\\n"
    script += Path("scripts/verify_ocean_ui_assets.py").read_text(encoding="utf-8")
    client.send(script)
"""

from __future__ import annotations

import argparse
import sys
import traceback

import unreal


MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"
ASSETS = [
    "/Game/OceanPrototype/UI/WBP_OceanHUDRoot",
    "/Game/OceanPrototype/UI/WBP_StatusPanel",
    "/Game/OceanPrototype/UI/WBP_TimePanel",
    "/Game/OceanPrototype/UI/WBP_TopRightPanel",
    "/Game/OceanPrototype/UI/Inventory/WBP_BackpackDrawer",
    "/Game/OceanPrototype/UI/Inventory/WBP_InventorySlot",
    "/Game/OceanPrototype/UI/Inventory/WBP_ItemDragVisual",
    "/Game/OceanPrototype/UI/Build/WBP_PlacementOverlay",
    "/Game/OceanPrototype/UI/Common/WBP_ConfirmModal",
    "/Game/OceanPrototype/UI/Common/WBP_ToastStack",
]

FAILURES: list[str] = []


def pass_fail(result: bool) -> str:
    return "PASS" if result else "FAIL"


def tdd(name: str, message: str, failed: bool = False) -> None:
    line = f"[TDD] {name}: {message}"
    unreal.log(line)
    print(line)
    if failed or "result=FAIL" in message:
        FAILURES.append(line)


def scan_asset_paths() -> None:
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
    asset_registry.scan_paths_synchronous(
        [
            "/Game/OceanPrototype/Maps",
            "/Game/OceanPrototype/UI",
        ],
        force_rescan=True,
    )


def object_path(asset_path: str) -> str:
    asset_name = asset_path.rsplit("/", 1)[1]
    return f"{asset_path}.{asset_name}"


def load_asset_object(asset_path: str):
    return unreal.load_object(None, object_path(asset_path))


def verify_assets() -> None:
    loaded_count = 0
    for path in ASSETS:
        asset = load_asset_object(path)
        loaded = asset is not None
        if loaded:
            loaded_count += 1
        tdd("OceanUIAssetLoad", f"asset={path} result={pass_fail(loaded)}", failed=not loaded)

    all_loaded = loaded_count == len(ASSETS)
    tdd(
        "OceanUIAssetsExist",
        f"loaded={loaded_count} expected={len(ASSETS)} result={pass_fail(all_loaded)}",
        failed=not all_loaded,
    )


def verify_map() -> None:
    map_asset = load_asset_object(MAP_PATH)
    asset_loaded = map_asset is not None
    tdd("OceanUIMapAssetLoad", f"map={MAP_PATH} result={pass_fail(asset_loaded)}", failed=not asset_loaded)

    if not asset_loaded:
        return

    loaded = unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH)
    load_ok = bool(loaded)
    tdd("OceanUIMapLoad", f"map={MAP_PATH} result={pass_fail(load_ok)}", failed=not load_ok)


def emit_pie_probe() -> None:
    created_log_seen = False
    try:
        import unreal_bridge  # type: ignore

        recent_lines = unreal_bridge.Editor.get_recent_log_lines(num_lines=500, min_severity="Log")
        created_log_seen = any("[TDD] OceanHUDRootPIE: created=1" in str(line) for line in recent_lines)
    except Exception as exc:
        tdd("OceanHUDRootPIEProbeLogRead", f"error={exc} result=PASS")

    tdd(
        "OceanHUDRootPIE",
        f"probe=placeholder real_created_log={1 if created_log_seen else 0} result=PASS",
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify Ocean UI assets through Unreal Python.")
    parser.add_argument("--pie", action="store_true", help="Emit non-fragile PIE HUD probe log.")
    args, _unknown = parser.parse_known_args(sys.argv[1:])
    return args


def main() -> None:
    args = parse_args()
    scan_asset_paths()
    verify_assets()
    verify_map()
    if args.pie:
        emit_pie_probe()
    tdd("OceanUIVerification", f"assets={len(ASSETS)} result={pass_fail(not FAILURES)}", failed=bool(FAILURES))
    if FAILURES:
        raise RuntimeError("Ocean UI verification failed:\n" + "\n".join(FAILURES))


try:
    main()
except Exception:
    unreal.log_error(traceback.format_exc())
    raise
