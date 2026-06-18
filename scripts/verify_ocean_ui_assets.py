"""
Verify Ocean HUD/backpack Widget Blueprint assets and WaterOcean map.

Run through UnrealBridge from the project root:
    from pathlib import Path
    from scripts.ue_tdd_bridge import BridgeClient

    client = BridgeClient()
    client.connect()
    client.send(Path("scripts/verify_ocean_ui_assets.py").read_text(encoding="utf-8"))

For the optional PIE probe:
    Run `python scripts/verify_ocean_ui_pie.py` first, or run another external
    Bridge/Pipeline probe that loads L_WaterOcean and starts PIE.

    source = Path("scripts/verify_ocean_ui_assets.py").read_text(encoding="utf-8")
    script = "OCEAN_VERIFY_UI_ARGS = ['--pie']\\n"
    script += "exec(" + repr(source) + ")"
    client.send(script)
"""

from __future__ import annotations

import argparse
import sys
import traceback

import unreal


MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"
HUD_ROOT_WIDGET_CLASS_PATH = "/Game/OceanPrototype/UI/WBP_OceanHUDRoot.WBP_OceanHUDRoot_C"
PLAYER_CONTROLLER_CLASS_PATH = "/Game/OceanPrototype/Blueprints/BP_OceanMVPPlayerController.BP_OceanMVPPlayerController_C"
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


def get_first_editor_property(obj, property_names: list[str]):
    for property_name in property_names:
        try:
            return obj.get_editor_property(property_name)
        except Exception:
            continue
    return None


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


def verify_hud_binding() -> None:
    hud_class = unreal.load_class(None, HUD_ROOT_WIDGET_CLASS_PATH)
    hud_class_loaded = hud_class is not None
    tdd(
        "OceanUIHUDRootGeneratedClass",
        f"class={HUD_ROOT_WIDGET_CLASS_PATH} result={pass_fail(hud_class_loaded)}",
        failed=not hud_class_loaded,
    )

    controller_class = unreal.load_class(None, PLAYER_CONTROLLER_CLASS_PATH)
    controller_class_loaded = controller_class is not None
    tdd(
        "OceanUIPlayerControllerClass",
        f"class={PLAYER_CONTROLLER_CLASS_PATH} result={pass_fail(controller_class_loaded)}",
        failed=not controller_class_loaded,
    )

    if hud_class is None or controller_class is None:
        return

    controller_cdo = unreal.get_default_object(controller_class)
    bound_class = get_first_editor_property(
        controller_cdo,
        ["hud_root_widget_class", "HUDRootWidgetClass", "HudRootWidgetClass"],
    )
    bound_path = bound_class.get_path_name() if hasattr(bound_class, "get_path_name") else "None"
    binding_ok = bound_class == hud_class
    tdd(
        "OceanUIHUDRootClassBinding",
        f"expected={HUD_ROOT_WIDGET_CLASS_PATH} actual={bound_path} result={pass_fail(binding_ok)}",
        failed=not binding_ok,
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
        tdd("OceanHUDRootPIEProbeLogRead", f"error={exc} result=FAIL", failed=True)
        return

    tdd(
        "OceanHUDRootPIE",
        f"real_created_log={1 if created_log_seen else 0} result={pass_fail(created_log_seen)}",
        failed=not created_log_seen,
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify Ocean UI assets through Unreal Python.")
    parser.add_argument("--pie", action="store_true", help="Emit non-fragile PIE HUD probe log.")
    injected_args = globals().get("OCEAN_VERIFY_UI_ARGS")
    raw_args = injected_args if injected_args is not None else sys.argv[1:]
    args, _unknown = parser.parse_known_args(raw_args)
    return args


def main() -> None:
    args = parse_args()
    scan_asset_paths()
    verify_assets()
    verify_hud_binding()
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
