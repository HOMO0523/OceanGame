"""
Verify the generated Ocean MVP survival-loop assets and starter map actors.

Run from Unreal Editor Python, or through UnrealBridge:
    UnrealEditor.exe Ocean.uproject -ExecutePythonScript=scripts/verify_mvp_survival_loop.py
"""

from __future__ import annotations

import traceback

import unreal


MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"

REQUIRED_ASSETS = [
    "/Game/OceanPrototype/Input/IA_OceanMove",
    "/Game/OceanPrototype/Input/IA_OceanInteract",
    "/Game/OceanPrototype/Input/IA_OceanToggleBuild",
    "/Game/OceanPrototype/Input/IA_OceanRotateBuild",
    "/Game/OceanPrototype/Input/IMC_OceanMVP",
    "/Game/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter",
    "/Game/OceanPrototype/Blueprints/BP_OceanMVPGameMode",
    "/Game/OceanPrototype/Build/DA_BuildModule_Deck_1x1",
]

REQUIRED_LABELS = [
    "OceanFloatingPlatform_Starter",
    "OceanResourceField_Starter",
    "PlayerStart_MVP",
]

FAILURES: list[str] = []


def tdd(name: str, message: str, failed: bool = False) -> None:
    line = f"[TDD] {name}: {message}"
    unreal.log(line)
    print(line)
    if failed or "result=FAIL" in message:
        FAILURES.append(line)


def pass_fail(result: bool) -> str:
    return "PASS" if result else "FAIL"


def get_level_subsystem():
    subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if subsystem is None:
        raise RuntimeError("LevelEditorSubsystem is unavailable")
    return subsystem


def load_map() -> None:
    exists = unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH)
    if not exists:
        tdd("MVPMapLoaded", f"path={MAP_PATH} result=FAIL", failed=True)
        raise RuntimeError(f"Required map does not exist: {MAP_PATH}")

    loaded = get_level_subsystem().load_level(MAP_PATH)
    tdd("MVPMapLoaded", f"path={MAP_PATH} result={pass_fail(bool(loaded))}", failed=not loaded)
    if not loaded:
        raise RuntimeError(f"Failed to load map: {MAP_PATH}")


def count_actors_by_class_name(class_name: str) -> int:
    count = 0
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        try:
            actor_class = actor.get_class()
            while actor_class is not None:
                if actor_class.get_name() == class_name:
                    count += 1
                    break
                actor_class = actor_class.get_super_class()
        except Exception:
            continue
    return count


def count_actors_by_label_prefix(prefix: str) -> int:
    count = 0
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        try:
            if actor.get_actor_label().startswith(prefix):
                count += 1
        except Exception:
            continue
    return count


def has_actor_label(label: str) -> bool:
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        try:
            if actor.get_actor_label() == label:
                return True
        except Exception:
            continue
    return False


def verify_assets() -> None:
    for asset_path in REQUIRED_ASSETS:
        exists = unreal.EditorAssetLibrary.does_asset_exist(asset_path)
        tdd("MVPAssetExists", f"path={asset_path} result={pass_fail(exists)}", failed=not exists)


def verify_actor_counts() -> None:
    for label in REQUIRED_LABELS:
        exists = has_actor_label(label)
        tdd("MVPActorLabelExists", f"label={label} result={pass_fail(exists)}", failed=not exists)

    platform_count = count_actors_by_class_name("OceanFloatingPlatform")
    resource_field_count = count_actors_by_class_name("OceanResourceField")
    resource_node_count = count_actors_by_label_prefix("OceanResourceNode_Starter_")

    tdd("MVPPlatformCount", f"actual={platform_count} expected>=1", failed=platform_count < 1)
    tdd("MVPResourceFieldCount", f"actual={resource_field_count} expected>=1", failed=resource_field_count < 1)
    tdd("MVPStarterResourceNodeCount", f"actual={resource_node_count} expected>=1", failed=resource_node_count < 1)


def verify_deck_definition() -> None:
    deck = unreal.EditorAssetLibrary.load_asset("/Game/OceanPrototype/Build/DA_BuildModule_Deck_1x1")
    if deck is None:
        tdd("MVPDeckDefinition", "result=FAIL", failed=True)
        return

    footprint = deck.get_editor_property("FootprintSize")
    build_cost = deck.get_editor_property("BuildCost")
    adjacency = deck.get_editor_property("bRequiresAdjacency")
    footprint_ok = footprint == unreal.IntPoint(1, 1)
    cost_amount = build_cost[0].get_editor_property("Amount") if build_cost else 0
    cost_ok = len(build_cost) == 1 and cost_amount == 2
    adjacency_ok = bool(adjacency)

    tdd("MVPDeckFootprint", f"actual={footprint.x}x{footprint.y} expected=1x1 result={pass_fail(footprint_ok)}", failed=not footprint_ok)
    tdd("MVPDeckWoodCost", f"actual={cost_amount} expected=2 result={pass_fail(cost_ok)}", failed=not cost_ok)
    tdd("MVPDeckRequiresAdjacency", f"actual={adjacency_ok} expected=True result={pass_fail(adjacency_ok)}", failed=not adjacency_ok)


def main() -> None:
    load_map()
    verify_assets()
    verify_actor_counts()
    verify_deck_definition()

    if FAILURES:
        raise RuntimeError("MVP verification failed:\n" + "\n".join(FAILURES))


try:
    main()
except Exception:
    unreal.log_error(traceback.format_exc())
    raise
