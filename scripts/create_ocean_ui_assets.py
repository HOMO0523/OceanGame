"""
Create Ocean HUD/backpack Widget Blueprint assets.

Run through UnrealBridge from the project root:
    BridgeClient().send(Path("scripts/create_ocean_ui_assets.py").read_text(encoding="utf-8"))
"""

from __future__ import annotations

import traceback

import unreal


UI_ROOT = "/Game/OceanPrototype/UI"
INVENTORY_DIR = f"{UI_ROOT}/Inventory"
BUILD_DIR = f"{UI_ROOT}/Build"
COMMON_DIR = f"{UI_ROOT}/Common"

ASSETS = {
    f"{UI_ROOT}/WBP_OceanHUDRoot": "/Script/Ocean.OceanHUDRootWidget",
    f"{UI_ROOT}/WBP_StatusPanel": "/Script/UMG.UserWidget",
    f"{UI_ROOT}/WBP_TimePanel": "/Script/UMG.UserWidget",
    f"{UI_ROOT}/WBP_TopRightPanel": "/Script/UMG.UserWidget",
    f"{INVENTORY_DIR}/WBP_BackpackDrawer": "/Script/UMG.UserWidget",
    f"{INVENTORY_DIR}/WBP_InventorySlot": "/Script/UMG.UserWidget",
    f"{INVENTORY_DIR}/WBP_ItemDragVisual": "/Script/UMG.UserWidget",
    f"{BUILD_DIR}/WBP_PlacementOverlay": "/Script/UMG.UserWidget",
    f"{COMMON_DIR}/WBP_ConfirmModal": "/Script/UMG.UserWidget",
    f"{COMMON_DIR}/WBP_ToastStack": "/Script/UMG.UserWidget",
}

FAILURES: list[str] = []


def pass_fail(result: bool) -> str:
    return "PASS" if result else "FAIL"


def tdd(name: str, message: str, failed: bool = False) -> None:
    line = f"[TDD] {name}: {message}"
    unreal.log(line)
    print(line)
    if failed or "result=FAIL" in message:
        FAILURES.append(line)


def ensure_dir(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
    exists = unreal.EditorAssetLibrary.does_directory_exist(path)
    tdd("OceanUIFolderExists", f"path={path} result={pass_fail(exists)}", failed=not exists)


def load_parent_class(parent_class_path: str):
    parent_class = unreal.load_class(None, parent_class_path)
    tdd(
        "OceanUIParentClassLoad",
        f"class={parent_class_path} result={pass_fail(parent_class is not None)}",
        failed=parent_class is None,
    )
    return parent_class


def asset_data_tag(path: str, tag_name: str) -> str:
    asset_data = unreal.EditorAssetLibrary.find_asset_data(path)
    if not asset_data.is_valid():
        return ""
    tag_value = asset_data.get_tag_value(tag_name)
    return str(tag_value) if tag_value else ""


def generated_class_path_from_tag(path: str) -> str:
    tag_value = asset_data_tag(path, "GeneratedClass")
    if "'" not in tag_value:
        return ""
    return tag_value.split("'", 2)[1]


def verify_widget_parent(path: str, parent_class) -> bool:
    widget_blueprint = unreal.EditorAssetLibrary.load_asset(path)
    if widget_blueprint is None or parent_class is None:
        return False

    parent_path = parent_class.get_path_name()
    parent_tag = asset_data_tag(path, "ParentClass") or asset_data_tag(path, "NativeParentClass")
    if f"'{parent_path}'" not in parent_tag:
        return False

    generated_class_path = generated_class_path_from_tag(path)
    if not generated_class_path:
        return False
    return unreal.load_class(None, generated_class_path) is not None


def ensure_widget(path: str, parent_class_path: str) -> None:
    parent_class = load_parent_class(parent_class_path)
    if parent_class is None:
        tdd("OceanUIAssetCreated", f"asset={path} result=FAIL", failed=True)
        return

    if unreal.EditorAssetLibrary.does_asset_exist(path):
        parent_ok = verify_widget_parent(path, parent_class)
        tdd("OceanUIAssetExists", f"asset={path} result={pass_fail(parent_ok)}", failed=not parent_ok)
        return

    package_path, asset_name = path.rsplit("/", 1)
    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset = asset_tools.create_asset(asset_name, package_path, unreal.WidgetBlueprint, factory)
    created = asset is not None and unreal.EditorAssetLibrary.does_asset_exist(path)
    if created and hasattr(unreal, "KismetEditorUtilities"):
        try:
            unreal.KismetEditorUtilities.compile_blueprint(asset)
        except Exception as exc:
            tdd("OceanUIAssetCompile", f"asset={path} error={exc} result=FAIL", failed=True)
    tdd("OceanUIAssetCreated", f"asset={path} result={pass_fail(created)}", failed=not created)


def save_and_verify() -> None:
    save_directory_result = unreal.EditorAssetLibrary.save_directory(UI_ROOT, only_if_is_dirty=True, recursive=True)
    save_packages_result = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    save_ok = bool(save_directory_result) and bool(save_packages_result)
    tdd("OceanUISaveDirtyPackages", f"result={pass_fail(save_ok)}", failed=not save_ok)

    for path, parent_class_path in ASSETS.items():
        asset = unreal.EditorAssetLibrary.load_asset(path)
        exists = asset is not None
        parent_class = unreal.load_class(None, parent_class_path)
        parent_ok = exists and verify_widget_parent(path, parent_class)
        tdd("OceanUIAssetLoad", f"asset={path} result={pass_fail(exists and parent_ok)}", failed=not (exists and parent_ok))


def main() -> None:
    for folder in (UI_ROOT, INVENTORY_DIR, BUILD_DIR, COMMON_DIR):
        ensure_dir(folder)
    for path, parent_class_path in ASSETS.items():
        ensure_widget(path, parent_class_path)
    save_and_verify()
    tdd("OceanUIAssetCreation", f"assets={len(ASSETS)} result={pass_fail(not FAILURES)}", failed=bool(FAILURES))
    if FAILURES:
        raise RuntimeError("Ocean UI asset creation failed:\n" + "\n".join(FAILURES))


try:
    main()
except Exception:
    unreal.log_error(traceback.format_exc())
    raise
