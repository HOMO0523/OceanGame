"""按 UI 设计规范创建 WBP 资产，删除旧资产后用正确 C++ 父类重建。"""
from __future__ import annotations
import unreal

UI_ROOT = "/Game/OceanPrototype/UI"
INVENTORY_DIR = f"{UI_ROOT}/Inventory"
BUILD_DIR = f"{UI_ROOT}/Build"
COMMON_DIR = f"{UI_ROOT}/Common"

# WBP路径 → C++ 父类路径 (None = UserWidget 通用)
ASSETS = {
    f"{UI_ROOT}/WBP_OceanHUDRoot":   "/Script/Ocean.OceanHUDRootWidget",
    f"{UI_ROOT}/WBP_StatusPanel":    "/Script/Ocean.OceanStatusPanelWidget",
    f"{UI_ROOT}/WBP_TimePanel":      "/Script/Ocean.OceanTimePanelWidget",
    f"{UI_ROOT}/WBP_TopRightPanel":  None,
    f"{INVENTORY_DIR}/WBP_BackpackDrawer": "/Script/Ocean.OceanBackpackPanelWidget",
    f"{INVENTORY_DIR}/WBP_InventorySlot":  "/Script/Ocean.OceanBackpackSlotWidget",
    f"{INVENTORY_DIR}/WBP_ItemDragVisual": None,
    f"{BUILD_DIR}/WBP_PlacementOverlay":   None,
    f"{BUILD_DIR}/WBP_BuildPalette":       "/Script/Ocean.OceanBuildPanelWidget",
    f"{COMMON_DIR}/WBP_ConfirmModal":      "/Script/Ocean.OceanItemUseModalWidget",
    f"{COMMON_DIR}/WBP_ToastStack":        "/Script/Ocean.OceanToastWidget",
}

# 需要用正确父类强制重建的资产（已存在但父类不对）
FORCE_RECREATE = {
    f"{UI_ROOT}/WBP_TimePanel",
    f"{UI_ROOT}/WBP_StatusPanel",
    f"{INVENTORY_DIR}/WBP_BackpackDrawer",
    f"{INVENTORY_DIR}/WBP_InventorySlot",
    f"{COMMON_DIR}/WBP_ConfirmModal",
    f"{COMMON_DIR}/WBP_ToastStack",
}

failures = []

def tdd(name, message, failed=False):
    line = f"[TDD] {name}: {message}"
    unreal.log(line)
    print(line)
    if failed or "result=FAIL" in message:
        failures.append(line)

def ensure_dir(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
    exists = unreal.EditorAssetLibrary.does_directory_exist(path)
    tdd("OceanUIFolderExists", f"path={path} result={'PASS' if exists else 'FAIL'}", failed=not exists)

def create_widget(path, parent_class_path):
    """创建 WBP，如果已存在且不在强制重建列表中则跳过。"""
    exists = unreal.EditorAssetLibrary.does_asset_exist(path)

    if exists and path not in FORCE_RECREATE:
        tdd("OceanUIAssetKeep", f"asset={path} parent={'UserWidget' if parent_class_path is None else parent_class_path.split('/')[-1]} result=PASS")
        return

    # 删除旧资产后重建
    if exists:
        unreal.EditorAssetLibrary.delete_asset(path)
        tdd("OceanUIAssetDelete", f"asset={path} (old parent) result=PASS")

    parent_class = None
    if parent_class_path is not None:
        parent_class = unreal.load_class(None, parent_class_path)
        if parent_class is None:
            tdd("OceanUIParentMissing", f"parent={parent_class_path} result=FAIL", failed=True)
            return

    factory = unreal.WidgetBlueprintFactory()
    if parent_class is not None:
        factory.set_editor_property("parent_class", parent_class)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    package_path, asset_name = path.rsplit("/", 1)
    asset = asset_tools.create_asset(asset_name, package_path, None, factory)
    ok = asset is not None
    parent_name = parent_class.get_name() if parent_class else "UserWidget"
    tdd("OceanUIAssetCreate", f"asset={path} parent={parent_name} result={'PASS' if ok else 'FAIL'}", failed=not ok)

def main():
    for folder in (UI_ROOT, INVENTORY_DIR, BUILD_DIR, COMMON_DIR):
        ensure_dir(folder)

    for path, parent in ASSETS.items():
        create_widget(path, parent)

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)

    for path in ASSETS:
        loaded = unreal.EditorAssetLibrary.load_asset(path) is not None
        tdd("OceanUIAssetLoad", f"asset={path} result={'PASS' if loaded else 'FAIL'}", failed=not loaded)

    print(f"\n{'='*50}\nTotal: {len(ASSETS)} assets | failures: {len(failures)}\n{'='*50}")

if __name__ == "__main__":
    main()
