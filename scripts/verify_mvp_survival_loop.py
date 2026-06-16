"""
Verify the generated Ocean MVP survival-loop assets and starter map actors.

Run from Unreal Editor Python, or through UnrealBridge:
    UnrealEditor.exe Ocean.uproject -ExecutePythonScript=scripts/verify_mvp_survival_loop.py
"""

from __future__ import annotations

import traceback

import unreal


MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"
INPUT_CONTEXT_PATH = "/Game/OceanPrototype/Input/IMC_OceanMVP"
SURVIVOR_BP_PATH = "/Game/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter"
PLAYER_CONTROLLER_BP_PATH = "/Game/OceanPrototype/Blueprints/BP_OceanMVPPlayerController"
GAMEMODE_BP_PATH = "/Game/OceanPrototype/Blueprints/BP_OceanMVPGameMode"
DECK_DEFINITION_PATH = "/Game/OceanPrototype/Build/DA_BuildModule_Deck_1x1"
SET_DESTINATION_CLICK_ACTION_PATH = "/Game/TopDown/Input/Actions/IA_SetDestination_Click"
SET_DESTINATION_TOUCH_ACTION_PATH = "/Game/TopDown/Input/Actions/IA_SetDestination_Touch"
EXPECTED_STARTER_RESOURCE_NODES = 16

REQUIRED_ASSETS = [
    "/Game/OceanPrototype/Input/IA_OceanMove",
    "/Game/OceanPrototype/Input/IA_OceanInteract",
    "/Game/OceanPrototype/Input/IA_OceanToggleBuild",
    "/Game/OceanPrototype/Input/IA_OceanRotateBuild",
    INPUT_CONTEXT_PATH,
    SET_DESTINATION_CLICK_ACTION_PATH,
    SET_DESTINATION_TOUCH_ACTION_PATH,
    SURVIVOR_BP_PATH,
    PLAYER_CONTROLLER_BP_PATH,
    GAMEMODE_BP_PATH,
    DECK_DEFINITION_PATH,
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


def get_class(script_path: str):
    loaded = unreal.load_class(None, script_path)
    if loaded is None:
        raise RuntimeError(f"Could not load class {script_path}")
    return loaded


def load_asset(asset_path: str):
    return unreal.EditorAssetLibrary.load_asset(asset_path)


def get_prop(obj, prop_names, default=None):
    if isinstance(prop_names, str):
        prop_names = [prop_names]

    for prop_name in prop_names:
        try:
            return obj.get_editor_property(prop_name)
        except Exception:
            continue
    return default


def key_name(value):
    name_value = get_prop(value, ["key_name", "KeyName"])
    if name_value is None:
        return None
    return str(name_value)


def object_identity(value):
    if value is None:
        return None
    for function_name in ("get_path_name", "get_name"):
        function = getattr(value, function_name, None)
        if function is None:
            continue
        try:
            return function()
        except Exception:
            continue
    return str(value)


def same_object(left, right) -> bool:
    return object_identity(left) == object_identity(right)


def get_input_context_mappings(mapping_context):
    direct_mappings = list(get_prop(mapping_context, ["mappings", "Mappings"], []))
    if direct_mappings:
        return direct_mappings

    default_mapping_data = get_prop(mapping_context, ["default_key_mappings", "DefaultKeyMappings"])
    if default_mapping_data is not None:
        return list(get_prop(default_mapping_data, ["mappings", "Mappings"], []))

    return []


def get_generated_class(blueprint_asset):
    library = getattr(unreal, "BlueprintEditorLibrary", None)
    if library and hasattr(library, "generated_class"):
        try:
            generated_class = library.generated_class(blueprint_asset)
            if generated_class is not None:
                return generated_class
        except Exception:
            pass

    generated_class = get_prop(blueprint_asset, ["generated_class", "GeneratedClass"])
    if generated_class is not None:
        return generated_class

    asset_path = unreal.EditorAssetLibrary.get_path_name_for_loaded_asset(blueprint_asset)
    if "." in asset_path:
        package_path, asset_name = asset_path.rsplit(".", 1)
    else:
        package_path = asset_path
        asset_name = asset_path.rsplit("/", 1)[-1]
    return unreal.load_class(None, f"{package_path}.{asset_name}_C")


def class_is_child_of(child_class, parent_class) -> bool:
    if child_class is None or parent_class is None:
        return False

    try:
        cdo = unreal.get_default_object(child_class)
        parent_python_class = getattr(unreal, parent_class.get_name(), None)
        if parent_python_class is not None and isinstance(cdo, parent_python_class):
            return True
    except Exception:
        pass

    current = child_class
    for _index in range(32):
        if current == parent_class:
            return True
        try:
            current = current.get_super_class()
        except Exception:
            break
        if current is None:
            break

    try:
        return bool(child_class.is_child_of(parent_class))
    except Exception:
        return False


def ocean_resource_type(name: str):
    enum_type = getattr(unreal, "OceanResourceType", None)
    if enum_type is None:
        return None
    for candidate in [name.upper(), name.capitalize(), name]:
        if hasattr(enum_type, candidate):
            return getattr(enum_type, candidate)
    return None


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
    resource_node_ok = resource_node_count == EXPECTED_STARTER_RESOURCE_NODES
    tdd("MVPStarterResourceNodeCount", f"actual={resource_node_count} expected={EXPECTED_STARTER_RESOURCE_NODES} result={pass_fail(resource_node_ok)}", failed=not resource_node_ok)


def verify_blueprint_parent(asset_path: str, parent_class_path: str) -> object:
    blueprint = load_asset(asset_path)
    parent_class = get_class(parent_class_path)
    generated_class = get_generated_class(blueprint) if blueprint else None
    ok = class_is_child_of(generated_class, parent_class)
    tdd("MVPBlueprintDerived", f"path={asset_path} parent={parent_class_path} result={pass_fail(ok)}", failed=not ok)
    return generated_class


def verify_input_context() -> None:
    context = load_asset(INPUT_CONTEXT_PATH)
    required_mappings = [
        ("/Game/OceanPrototype/Input/IA_OceanMove", "W"),
        ("/Game/OceanPrototype/Input/IA_OceanMove", "A"),
        ("/Game/OceanPrototype/Input/IA_OceanMove", "S"),
        ("/Game/OceanPrototype/Input/IA_OceanMove", "D"),
        ("/Game/OceanPrototype/Input/IA_OceanInteract", "F"),
        ("/Game/OceanPrototype/Input/IA_OceanToggleBuild", "B"),
        ("/Game/OceanPrototype/Input/IA_OceanRotateBuild", "R"),
        (SET_DESTINATION_CLICK_ACTION_PATH, "LeftMouseButton"),
        (SET_DESTINATION_TOUCH_ACTION_PATH, "Touch1"),
    ]

    if context is None:
        tdd("MVPInputContext", f"path={INPUT_CONTEXT_PATH} result=FAIL", failed=True)
        return

    mappings = get_input_context_mappings(context)
    for action_path, expected_key in required_mappings:
        action = load_asset(action_path)
        found = False
        for mapping in mappings:
            if same_object(get_prop(mapping, ["action", "Action"]), action) and key_name(get_prop(mapping, ["key", "Key"])) == expected_key:
                found = True
                break
        tdd("MVPInputMapping", f"action={action_path.rsplit('/', 1)[-1]} key={expected_key} result={pass_fail(found)}", failed=not found)

    count_ok = len(mappings) == len(required_mappings)
    tdd("MVPInputContext", f"path={INPUT_CONTEXT_PATH} mappings={len(mappings)} expected={len(required_mappings)} result={pass_fail(count_ok)}", failed=not count_ok)


def verify_controller_and_game_mode() -> None:
    survivor_class = verify_blueprint_parent(SURVIVOR_BP_PATH, "/Script/Ocean.OceanCharacter")
    controller_class = verify_blueprint_parent(PLAYER_CONTROLLER_BP_PATH, "/Script/Ocean.OceanPlayerController")
    game_mode_class = verify_blueprint_parent(GAMEMODE_BP_PATH, "/Script/Ocean.OceanGameMode")

    if controller_class is not None:
        controller_cdo = unreal.get_default_object(controller_class)
        checks = {
            "DefaultMappingContext": same_object(get_prop(controller_cdo, ["default_mapping_context", "DefaultMappingContext"]), load_asset(INPUT_CONTEXT_PATH)),
            "SetDestinationClickAction": same_object(get_prop(controller_cdo, ["set_destination_click_action", "SetDestinationClickAction"]), load_asset(SET_DESTINATION_CLICK_ACTION_PATH)),
            "SetDestinationTouchAction": same_object(get_prop(controller_cdo, ["set_destination_touch_action", "SetDestinationTouchAction"]), load_asset(SET_DESTINATION_TOUCH_ACTION_PATH)),
            "MoveAction": same_object(get_prop(controller_cdo, ["move_action", "MoveAction"]), load_asset("/Game/OceanPrototype/Input/IA_OceanMove")),
            "InteractAction": same_object(get_prop(controller_cdo, ["interact_action", "InteractAction"]), load_asset("/Game/OceanPrototype/Input/IA_OceanInteract")),
            "ToggleBuildAction": same_object(get_prop(controller_cdo, ["toggle_build_action", "ToggleBuildAction"]), load_asset("/Game/OceanPrototype/Input/IA_OceanToggleBuild")),
            "RotateBuildAction": same_object(get_prop(controller_cdo, ["rotate_build_action", "RotateBuildAction"]), load_asset("/Game/OceanPrototype/Input/IA_OceanRotateBuild")),
        }
        for prop_name, ok in checks.items():
            tdd("MVPPlayerControllerInput", f"property={prop_name} result={pass_fail(ok)}", failed=not ok)

    if game_mode_class is not None:
        game_mode_cdo = unreal.get_default_object(game_mode_class)
        pawn_ok = same_object(get_prop(game_mode_cdo, ["default_pawn_class", "DefaultPawnClass"]), survivor_class)
        controller_ok = same_object(get_prop(game_mode_cdo, ["player_controller_class", "PlayerControllerClass"]), controller_class)
        tdd("MVPGameModeDefaultPawn", f"default_pawn={SURVIVOR_BP_PATH} result={pass_fail(pawn_ok)}", failed=not pawn_ok)
        tdd("MVPGameModePlayerController", f"player_controller={PLAYER_CONTROLLER_BP_PATH} result={pass_fail(controller_ok)}", failed=not controller_ok)


def verify_deck_definition() -> None:
    deck = load_asset(DECK_DEFINITION_PATH)
    if deck is None:
        tdd("MVPDeckDefinition", "result=FAIL", failed=True)
        return

    footprint = deck.get_editor_property("FootprintSize")
    build_cost = deck.get_editor_property("BuildCost")
    adjacency = deck.get_editor_property("bRequiresAdjacency")
    preview_mesh = get_prop(deck, ["preview_mesh", "PreviewMesh"])
    module_actor_class = get_prop(deck, ["module_actor_class", "ModuleActorClass"])
    footprint_ok = footprint == unreal.IntPoint(1, 1)
    cost_amount = build_cost[0].get_editor_property("Amount") if build_cost else 0
    cost_resource = get_prop(build_cost[0], ["resource_type", "ResourceType"]) if build_cost else None
    wood_type = ocean_resource_type("Wood")
    cost_resource_ok = cost_resource == wood_type
    cost_ok = len(build_cost) == 1 and cost_amount == 2 and cost_resource_ok
    adjacency_ok = bool(adjacency)
    preview_mesh_ok = same_object(preview_mesh, unreal.load_object(None, "/Engine/BasicShapes/Cube.Cube"))
    module_actor_ok = same_object(module_actor_class, get_class("/Script/Ocean.OceanBuildModuleActor"))

    tdd("MVPDeckFootprint", f"actual={footprint.x}x{footprint.y} expected=1x1 result={pass_fail(footprint_ok)}", failed=not footprint_ok)
    tdd("MVPDeckWoodCost", f"actual={cost_amount} expected=2 result={pass_fail(cost_ok)}", failed=not cost_ok)
    tdd("MVPDeckResourceType", f"actual={cost_resource} expected=Wood result={pass_fail(cost_resource_ok)}", failed=not cost_resource_ok)
    tdd("MVPDeckRequiresAdjacency", f"actual={adjacency_ok} expected=True result={pass_fail(adjacency_ok)}", failed=not adjacency_ok)
    tdd("MVPDeckPreviewMesh", f"mesh=/Engine/BasicShapes/Cube result={pass_fail(preview_mesh_ok)}", failed=not preview_mesh_ok)
    tdd("MVPDeckModuleActorClass", f"class=/Script/Ocean.OceanBuildModuleActor result={pass_fail(module_actor_ok)}", failed=not module_actor_ok)


def main() -> None:
    load_map()
    verify_assets()
    verify_input_context()
    verify_controller_and_game_mode()
    verify_actor_counts()
    verify_deck_definition()

    if FAILURES:
        raise RuntimeError("MVP verification failed:\n" + "\n".join(FAILURES))


try:
    main()
except Exception:
    unreal.log_error(traceback.format_exc())
    raise
