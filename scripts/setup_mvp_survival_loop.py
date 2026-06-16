"""
Set up the Ocean MVP survival-loop assets and starter map actors.

Run from Unreal Editor Python, or through UnrealBridge:
    UnrealEditor.exe Ocean.uproject -ExecutePythonScript=scripts/setup_mvp_survival_loop.py

Creates or updates:
    /Game/OceanPrototype/Input/IA_OceanMove
    /Game/OceanPrototype/Input/IA_OceanInteract
    /Game/OceanPrototype/Input/IA_OceanToggleBuild
    /Game/OceanPrototype/Input/IA_OceanRotateBuild
    /Game/OceanPrototype/Input/IMC_OceanMVP
    /Game/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter
    /Game/OceanPrototype/Blueprints/BP_OceanMVPGameMode
    /Game/OceanPrototype/Build/DA_BuildModule_Deck_1x1
    OceanFloatingPlatform_Starter
    OceanResourceField_Starter
    PlayerStart_MVP
"""

from __future__ import annotations

import traceback

import unreal


MAP_PATH = "/Game/OceanPrototype/Maps/L_WaterOcean"

INPUT_DIR = "/Game/OceanPrototype/Input"
BLUEPRINT_DIR = "/Game/OceanPrototype/Blueprints"
BUILD_DIR = "/Game/OceanPrototype/Build"

MOVE_ACTION_PATH = f"{INPUT_DIR}/IA_OceanMove"
INTERACT_ACTION_PATH = f"{INPUT_DIR}/IA_OceanInteract"
TOGGLE_BUILD_ACTION_PATH = f"{INPUT_DIR}/IA_OceanToggleBuild"
ROTATE_BUILD_ACTION_PATH = f"{INPUT_DIR}/IA_OceanRotateBuild"
INPUT_CONTEXT_PATH = f"{INPUT_DIR}/IMC_OceanMVP"
SURVIVOR_BP_PATH = f"{BLUEPRINT_DIR}/BP_OceanSurvivorCharacter"
GAMEMODE_BP_PATH = f"{BLUEPRINT_DIR}/BP_OceanMVPGameMode"
DECK_DEFINITION_PATH = f"{BUILD_DIR}/DA_BuildModule_Deck_1x1"

STARTER_PLATFORM_LABEL = "OceanFloatingPlatform_Starter"
STARTER_RESOURCE_FIELD_LABEL = "OceanResourceField_Starter"
STARTER_PLAYER_START_LABEL = "PlayerStart_MVP"
STARTER_RESOURCE_NODE_PREFIX = "OceanResourceNode_Starter_"

FALLBACK_RESOURCE_COUNT = 16
RESOURCE_FIELD_RADIUS = 3600.0
RESOURCE_EXCLUSION_RADIUS = 900.0

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


def get_asset_name(asset_path: str) -> str:
    return asset_path.rsplit("/", 1)[-1]


def get_asset_dir(asset_path: str) -> str:
    return asset_path.rsplit("/", 1)[0]


def get_field(value, *names, default=None):
    for name in names:
        if hasattr(value, name):
            return getattr(value, name)
        title_name = name[:1].upper() + name[1:]
        if hasattr(value, title_name):
            return getattr(value, title_name)
    return default


def nearly_equal_number(current, desired, tolerance: float = 0.01) -> bool:
    try:
        return abs(float(current) - float(desired)) <= tolerance
    except Exception:
        return False


def vector_nearly_equal(current, desired, tolerance: float = 0.1) -> bool:
    fields = ("x", "y", "z")
    return all(nearly_equal_number(get_field(current, field), get_field(desired, field), tolerance) for field in fields)


def rotator_nearly_equal(current, desired, tolerance: float = 0.1) -> bool:
    fields = ("pitch", "yaw", "roll")
    return all(nearly_equal_number(get_field(current, field), get_field(desired, field), tolerance) for field in fields)


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


def resource_stack_equal(current, desired) -> bool:
    current_amount = get_prop(current, ["amount", "Amount"])
    desired_amount = get_prop(desired, ["amount", "Amount"])
    current_type = get_prop(current, ["resource_type", "ResourceType"])
    desired_type = get_prop(desired, ["resource_type", "ResourceType"])
    return current_amount == desired_amount and current_type == desired_type


def values_equal(current, desired) -> bool:
    if current is desired or current == desired:
        return True

    if isinstance(current, float) or isinstance(desired, float):
        return nearly_equal_number(current, desired)

    if all(get_field(value, "x", default=None) is not None and get_field(value, "y", default=None) is not None for value in (current, desired)):
        if all(get_field(value, "z", default=None) is not None for value in (current, desired)):
            return vector_nearly_equal(current, desired)
        return get_field(current, "x") == get_field(desired, "x") and get_field(current, "y") == get_field(desired, "y")

    if all(get_field(value, "pitch", default=None) is not None for value in (current, desired)):
        return rotator_nearly_equal(current, desired)

    if key_name(current) is not None or key_name(desired) is not None:
        return key_name(current) == key_name(desired)

    if all(get_prop(value, ["amount", "Amount"]) is not None for value in (current, desired)):
        return resource_stack_equal(current, desired)

    if isinstance(current, (list, tuple)) and isinstance(desired, (list, tuple)):
        return len(current) == len(desired) and all(values_equal(left, right) for left, right in zip(current, desired))

    return object_identity(current) == object_identity(desired)


def ensure_folder(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
    tdd("MVPFolderExists", f"path={path} result={pass_fail(unreal.EditorAssetLibrary.does_directory_exist(path))}")


def set_prop(obj, prop_names, value, required: bool = False, label: str = "") -> bool:
    if isinstance(prop_names, str):
        prop_names = [prop_names]

    for prop_name in prop_names:
        try:
            current_value = obj.get_editor_property(prop_name)
            if values_equal(current_value, value):
                return True
            obj.set_editor_property(prop_name, value)
            return True
        except Exception:
            continue

    if required:
        tdd(
            "MVPPropertySet",
            f"target={label or obj.get_name()} props={','.join(prop_names)} result=FAIL",
            failed=True,
        )
    return False


def get_prop(obj, prop_names, default=None):
    if isinstance(prop_names, str):
        prop_names = [prop_names]

    for prop_name in prop_names:
        try:
            return obj.get_editor_property(prop_name)
        except Exception:
            continue
    return default


def call_if_exists(obj, method_names, *args):
    if isinstance(method_names, str):
        method_names = [method_names]

    for method_name in method_names:
        method = getattr(obj, method_name, None)
        if method is None:
            continue
        try:
            return True, method(*args)
        except Exception:
            continue
    return False, None


def get_level_subsystem():
    subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if subsystem is None:
        raise RuntimeError("LevelEditorSubsystem is unavailable")
    return subsystem


def load_map() -> None:
    if not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        tdd("MVPMapLoaded", f"path={MAP_PATH} result=FAIL", failed=True)
        raise RuntimeError(f"Required map does not exist: {MAP_PATH}")

    loaded = get_level_subsystem().load_level(MAP_PATH)
    tdd("MVPMapLoaded", f"path={MAP_PATH} result={pass_fail(bool(loaded))}")
    if not loaded:
        raise RuntimeError(f"Failed to load map: {MAP_PATH}")


def create_asset(asset_path: str, asset_class, factory):
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset = asset_tools.create_asset(
        get_asset_name(asset_path),
        get_asset_dir(asset_path),
        asset_class,
        factory,
    )
    if asset is None:
        raise RuntimeError(f"Failed to create asset {asset_path}")
    return asset


def create_or_load_asset(asset_path: str, asset_class, factory):
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        asset = load_asset(asset_path)
        if asset is None:
            raise RuntimeError(f"Failed to load existing asset {asset_path}")
        return asset, False

    asset = create_asset(asset_path, asset_class, factory)
    return asset, True


def input_action_value(value_name: str):
    enum_type = getattr(unreal, "InputActionValueType", None)
    if enum_type is None:
        return None

    candidate_map = {
        "Axis2D": ["AXIS2D", "Axis2D", "AXIS_2D"],
        "Boolean": ["BOOLEAN", "Boolean", "BOOL", "Bool"],
    }
    for candidate in candidate_map[value_name]:
        if hasattr(enum_type, candidate):
            return getattr(enum_type, candidate)
    return None


def create_input_action_factory():
    factory_class = getattr(unreal, "InputActionFactory", None) or getattr(unreal, "InputAction_Factory", None)
    if factory_class is None:
        return None
    return factory_class()


def ensure_input_action(asset_path: str, value_type_name: str):
    input_action_class = getattr(unreal, "InputAction", None) or get_class("/Script/EnhancedInput.InputAction")
    factory = create_input_action_factory()
    if factory is None and not unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        tdd("MVPInputActionFactory", f"path={asset_path} result=FAIL", failed=True)
        raise RuntimeError("InputActionFactory is unavailable")

    action, _created = create_or_load_asset(asset_path, input_action_class, factory)
    desired_value = input_action_value(value_type_name)
    if desired_value is None:
        tdd("MVPInputValueType", f"path={asset_path} expected={value_type_name} result=FAIL", failed=True)
        raise RuntimeError(f"InputActionValueType.{value_type_name} is unavailable")

    value_set = set_prop(action, ["value_type", "ValueType"], desired_value, required=True, label=asset_path)
    current_value = get_prop(action, ["value_type", "ValueType"])
    ok = value_set and current_value == desired_value
    tdd("MVPInputAction", f"path={asset_path} value_type={value_type_name} result={pass_fail(ok)}", failed=not ok)
    return action


def clear_input_context_mappings(mapping_context) -> bool:
    ok, _ = call_if_exists(mapping_context, ["unmap_all", "UnmapAll"])
    if ok:
        return True
    return set_prop(mapping_context, ["mappings", "Mappings"], [], required=True, label=INPUT_CONTEXT_PATH)


def make_key(key_name: str):
    try:
        key = unreal.Key()
        set_prop(key, ["key_name", "KeyName"], unreal.Name(key_name), required=True, label=f"Key:{key_name}")
        return key
    except Exception as exc:
        raise RuntimeError(f"Could not create key {key_name}: {exc}") from exc


def make_action_mapping(action, key_name: str):
    mapping_struct = getattr(unreal, "EnhancedActionKeyMapping", None)
    if mapping_struct is None:
        return None

    mapping = mapping_struct()
    set_prop(mapping, ["action", "Action"], action, required=True, label=f"{action.get_name()}:{key_name}")
    set_prop(mapping, ["key", "Key"], make_key(key_name), required=True, label=f"{action.get_name()}:{key_name}")
    return mapping


def action_matches(current_action, desired_action) -> bool:
    return object_identity(current_action) == object_identity(desired_action)


def mapping_matches(mapping, action, key_name_to_match: str) -> bool:
    mapped_action = get_prop(mapping, ["action", "Action"])
    mapped_key = get_prop(mapping, ["key", "Key"])
    return action_matches(mapped_action, action) and key_name(mapped_key) == key_name_to_match


def input_context_has_exact_mappings(mapping_context, desired_mappings) -> bool:
    current_mappings = list(get_prop(mapping_context, ["mappings", "Mappings"], []))
    if len(current_mappings) != len(desired_mappings):
        return False

    for action, key_name_to_match in desired_mappings:
        if not any(mapping_matches(mapping, action, key_name_to_match) for mapping in current_mappings):
            return False
    return True


def map_input_key(mapping_context, action, key_name: str) -> bool:
    ok, _ = call_if_exists(mapping_context, ["map_key", "MapKey"], action, make_key(key_name))
    if ok:
        tdd("MVPInputMapping", f"action={action.get_name()} key={key_name} result=PASS")
        return True

    current_mappings = list(get_prop(mapping_context, ["mappings", "Mappings"], []))
    mapping = make_action_mapping(action, key_name)
    if mapping is None:
        tdd("MVPInputMapping", f"action={action.get_name()} key={key_name} result=FAIL", failed=True)
        return False

    current_mappings.append(mapping)
    ok = set_prop(mapping_context, ["mappings", "Mappings"], current_mappings, required=True, label=INPUT_CONTEXT_PATH)
    tdd("MVPInputMapping", f"action={action.get_name()} key={key_name} result={pass_fail(ok)}", failed=not ok)
    return ok


def ensure_input_mapping_context(move_action, interact_action, toggle_build_action, rotate_build_action):
    mapping_context_class = getattr(unreal, "InputMappingContext", None) or get_class("/Script/EnhancedInput.InputMappingContext")
    factory_class = getattr(unreal, "InputMappingContextFactory", None) or getattr(unreal, "InputMappingContext_Factory", None)
    factory = factory_class() if factory_class is not None else None

    if factory is None and not unreal.EditorAssetLibrary.does_asset_exist(INPUT_CONTEXT_PATH):
        tdd("MVPInputMappingContextFactory", f"path={INPUT_CONTEXT_PATH} result=FAIL", failed=True)
        raise RuntimeError("InputMappingContextFactory is unavailable")

    mapping_context, _created = create_or_load_asset(INPUT_CONTEXT_PATH, mapping_context_class, factory)
    mappings = [
        (move_action, "W"),
        (move_action, "A"),
        (move_action, "S"),
        (move_action, "D"),
        (interact_action, "F"),
        (toggle_build_action, "B"),
        (rotate_build_action, "R"),
    ]

    if input_context_has_exact_mappings(mapping_context, mappings):
        for action, key_name_to_log in mappings:
            tdd("MVPInputMapping", f"action={action.get_name()} key={key_name_to_log} result=PASS")
        tdd("MVPInputContext", f"path={INPUT_CONTEXT_PATH} mappings={len(mappings)} result=PASS")
        return mapping_context

    clear_input_context_mappings(mapping_context)
    results = [map_input_key(mapping_context, action, key_name) for action, key_name in mappings]
    ok = all(results)
    tdd("MVPInputContext", f"path={INPUT_CONTEXT_PATH} mappings={len(mappings)} result={pass_fail(ok)}", failed=not ok)
    return mapping_context


def ensure_input_assets() -> None:
    ensure_folder(INPUT_DIR)
    move_action = ensure_input_action(MOVE_ACTION_PATH, "Axis2D")
    interact_action = ensure_input_action(INTERACT_ACTION_PATH, "Boolean")
    toggle_build_action = ensure_input_action(TOGGLE_BUILD_ACTION_PATH, "Boolean")
    rotate_build_action = ensure_input_action(ROTATE_BUILD_ACTION_PATH, "Boolean")
    ensure_input_mapping_context(move_action, interact_action, toggle_build_action, rotate_build_action)


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


def create_blueprint_factory(parent_class):
    factory = unreal.BlueprintFactory()
    set_prop(factory, ["parent_class", "ParentClass"], parent_class, required=True, label="BlueprintFactory")
    return factory


def ensure_blueprint(asset_path: str, parent_class_path: str):
    parent_class = get_class(parent_class_path)
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        blueprint_asset = load_asset(asset_path)
        if blueprint_asset is None:
            raise RuntimeError(f"Failed to load existing blueprint {asset_path}")
        generated_class = get_generated_class(blueprint_asset)
        if not class_is_child_of(generated_class, parent_class):
            library = getattr(unreal, "BlueprintEditorLibrary", None)
            reparented = False
            if library and hasattr(library, "reparent_blueprint"):
                try:
                    library.reparent_blueprint(blueprint_asset, parent_class)
                    reparented = True
                except Exception:
                    reparented = False
            if not reparented:
                tdd("MVPBlueprintDerived", f"path={asset_path} parent={parent_class_path} result=FAIL", failed=True)
                raise RuntimeError(f"Existing blueprint is not derived from {parent_class_path}: {asset_path}")
    else:
        blueprint_asset = create_asset(asset_path, unreal.Blueprint, create_blueprint_factory(parent_class))

    library = getattr(unreal, "BlueprintEditorLibrary", None)
    if library and hasattr(library, "compile_blueprint"):
        library.compile_blueprint(blueprint_asset)

    generated_class = get_generated_class(blueprint_asset)
    ok = class_is_child_of(generated_class, parent_class)
    tdd("MVPBlueprintDerived", f"path={asset_path} parent={parent_class_path} result={pass_fail(ok)}", failed=not ok)
    return blueprint_asset


def make_text(value: str):
    text_class = getattr(unreal, "Text", None)
    if text_class is None:
        return value
    try:
        return text_class(value)
    except Exception:
        return value


def ocean_resource_type(name: str):
    enum_type = getattr(unreal, "OceanResourceType", None)
    if enum_type is None:
        return None
    for candidate in [name.upper(), name.capitalize(), name]:
        if hasattr(enum_type, candidate):
            return getattr(enum_type, candidate)
    return None


def make_resource_stack(resource_type_name: str, amount: int):
    stack_class = getattr(unreal, "OceanResourceStack", None)
    if stack_class is None:
        raise RuntimeError("OceanResourceStack Python struct is unavailable")

    resource_type = ocean_resource_type(resource_type_name)
    if resource_type is None:
        raise RuntimeError(f"OceanResourceType.{resource_type_name} is unavailable")

    stack = stack_class()
    set_prop(stack, ["resource_type", "ResourceType"], resource_type, required=True, label="OceanResourceStack")
    set_prop(stack, ["amount", "Amount"], amount, required=True, label="OceanResourceStack")
    return stack


def create_data_asset_factory(data_asset_class):
    factory_class = getattr(unreal, "DataAssetFactory", None)
    if factory_class is None:
        return None
    factory = factory_class()
    set_prop(factory, ["data_asset_class", "DataAssetClass"], data_asset_class, required=True, label="DataAssetFactory")
    return factory


def ensure_deck_definition():
    ensure_folder(BUILD_DIR)
    definition_class = get_class("/Script/Ocean.OceanBuildModuleDefinition")
    factory = create_data_asset_factory(definition_class)
    if factory is None and not unreal.EditorAssetLibrary.does_asset_exist(DECK_DEFINITION_PATH):
        tdd("MVPDataAssetFactory", f"path={DECK_DEFINITION_PATH} result=FAIL", failed=True)
        raise RuntimeError("DataAssetFactory is unavailable")

    deck_definition, _created = create_or_load_asset(DECK_DEFINITION_PATH, definition_class, factory)
    cube_mesh = unreal.load_object(None, "/Engine/BasicShapes/Cube.Cube")
    module_actor_class = get_class("/Script/Ocean.OceanBuildModuleActor")
    build_cost = [make_resource_stack("Wood", 2)]

    set_prop(deck_definition, ["display_name", "DisplayName"], make_text("Deck 1x1"), required=True, label=DECK_DEFINITION_PATH)
    set_prop(deck_definition, ["footprint_size", "FootprintSize"], unreal.IntPoint(1, 1), required=True, label=DECK_DEFINITION_PATH)
    set_prop(deck_definition, ["build_cost", "BuildCost"], build_cost, required=True, label=DECK_DEFINITION_PATH)
    set_prop(deck_definition, ["preview_mesh", "PreviewMesh"], cube_mesh, required=True, label=DECK_DEFINITION_PATH)
    set_prop(deck_definition, ["module_actor_class", "ModuleActorClass"], module_actor_class, required=True, label=DECK_DEFINITION_PATH)
    set_prop(deck_definition, ["requires_adjacency", "b_requires_adjacency", "bRequiresAdjacency"], True, required=True, label=DECK_DEFINITION_PATH)
    set_prop(deck_definition, ["can_start_on_water", "b_can_start_on_water", "bCanStartOnWater"], False, required=False, label=DECK_DEFINITION_PATH)

    footprint = get_prop(deck_definition, ["footprint_size", "FootprintSize"])
    cost = get_prop(deck_definition, ["build_cost", "BuildCost"], [])
    cost_ok = len(cost) == 1 and get_prop(cost[0], ["amount", "Amount"], 0) == 2
    footprint_ok = footprint == unreal.IntPoint(1, 1)
    adjacency_ok = bool(get_prop(deck_definition, ["requires_adjacency", "b_requires_adjacency", "bRequiresAdjacency"], False))

    tdd("MVPDeckDefinitionFootprint", f"actual={footprint.x}x{footprint.y} expected=1x1 result={pass_fail(footprint_ok)}", failed=not footprint_ok)
    tdd("MVPDeckDefinitionCost", f"resource=Wood actual={get_prop(cost[0], ['amount', 'Amount'], 0) if cost else 0} expected=2 result={pass_fail(cost_ok)}", failed=not cost_ok)
    tdd("MVPDeckDefinitionAdjacency", f"actual={adjacency_ok} expected=True result={pass_fail(adjacency_ok)}", failed=not adjacency_ok)
    return deck_definition


def configure_game_mode_blueprint(game_mode_bp, survivor_bp) -> None:
    game_mode_class = get_generated_class(game_mode_bp)
    survivor_class = get_generated_class(survivor_bp)
    if game_mode_class is None or survivor_class is None:
        tdd("MVPGameModeDefaults", "result=FAIL", failed=True)
        return

    cdo = unreal.get_default_object(game_mode_class)
    ok = set_prop(cdo, ["default_pawn_class", "DefaultPawnClass"], survivor_class, required=True, label=GAMEMODE_BP_PATH)
    tdd("MVPGameModeDefaultPawn", f"default_pawn={SURVIVOR_BP_PATH} result={pass_fail(ok)}", failed=not ok)

    world = unreal.EditorLevelLibrary.get_editor_world()
    world_settings = world.get_world_settings() if world else None
    if world_settings is not None:
        map_ok = set_prop(world_settings, ["default_game_mode", "DefaultGameMode"], game_mode_class, required=True, label=MAP_PATH)
        tdd("MVPMapGameMode", f"game_mode={GAMEMODE_BP_PATH} result={pass_fail(map_ok)}", failed=not map_ok)


def ensure_blueprints_and_data() -> None:
    ensure_folder(BLUEPRINT_DIR)
    survivor_bp = ensure_blueprint(SURVIVOR_BP_PATH, "/Script/Ocean.OceanCharacter")
    game_mode_bp = ensure_blueprint(GAMEMODE_BP_PATH, "/Script/Ocean.OceanGameMode")
    ensure_deck_definition()
    configure_game_mode_blueprint(game_mode_bp, survivor_bp)


def find_actor_by_label(label: str):
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        try:
            if actor.get_actor_label() == label:
                return actor
        except Exception:
            continue
    return None


def move_actor_if_needed(actor, location: unreal.Vector, rotation: unreal.Rotator | None = None) -> None:
    if not vector_nearly_equal(actor.get_actor_location(), location):
        actor.set_actor_location(location, False, False)
    if rotation is not None and not rotator_nearly_equal(actor.get_actor_rotation(), rotation):
        actor.set_actor_rotation(rotation, False)


def find_actors_by_label_prefix(prefix: str):
    actors = []
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        try:
            if actor.get_actor_label().startswith(prefix):
                actors.append(actor)
        except Exception:
            continue
    return actors


def spawn_or_get_actor(label: str, class_path: str, location: unreal.Vector, rotation: unreal.Rotator | None = None):
    existing = find_actor_by_label(label)
    if existing is not None:
        move_actor_if_needed(existing, location, rotation)
        tdd("MVPActorExists", f"label={label} result=PASS")
        return existing, False

    actor_class = get_class(class_path)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        location,
        rotation or unreal.Rotator(0.0, 0.0, 0.0),
    )
    if actor is None:
        tdd("MVPActorExists", f"label={label} result=FAIL", failed=True)
        raise RuntimeError(f"Failed to spawn {label} from {class_path}")
    actor.set_actor_label(label)
    tdd("MVPActorExists", f"label={label} result=PASS")
    return actor, True


def configure_resource_node(node, index: int) -> None:
    amount = 2 if index % 4 == 0 else 1
    resource_type = ocean_resource_type("Wood")
    if resource_type is not None:
        set_prop(node, ["resource_type", "ResourceType"], resource_type, required=False, label=node.get_actor_label())
    set_prop(node, ["amount", "Amount"], amount, required=False, label=node.get_actor_label())


def spawn_starter_resource_nodes(resource_field) -> int:
    node_class = get_class("/Script/Ocean.OceanResourceNode")
    ok, locations = call_if_exists(resource_field, ["generate_fallback_resource_locations", "GenerateFallbackResourceLocations"], resource_field.get_actor_location())
    if not ok or locations is None:
        tdd("MVPResourceLocations", f"actual=0 expected={FALLBACK_RESOURCE_COUNT} result=FAIL", failed=True)
        return 0

    existing_nodes = {actor.get_actor_label(): actor for actor in find_actors_by_label_prefix(STARTER_RESOURCE_NODE_PREFIX)}
    desired_labels = set()
    spawned_count = 0
    for index, location in enumerate(list(locations)[:FALLBACK_RESOURCE_COUNT]):
        label = f"{STARTER_RESOURCE_NODE_PREFIX}{index:02d}"
        desired_labels.add(label)
        node = existing_nodes.get(label)
        if node is None:
            node = unreal.EditorLevelLibrary.spawn_actor_from_class(node_class, location, unreal.Rotator(0.0, 0.0, 0.0))
            if node is None:
                continue
            node.set_actor_label(label)
        else:
            move_actor_if_needed(node, location, unreal.Rotator(0.0, 0.0, 0.0))
        configure_resource_node(node, index)
        spawned_count += 1

    for label, node in existing_nodes.items():
        if label not in desired_labels:
            unreal.EditorLevelLibrary.destroy_actor(node)

    tdd("MVPResourceNodeCount", f"actual={spawned_count} expected={FALLBACK_RESOURCE_COUNT}")
    if spawned_count != FALLBACK_RESOURCE_COUNT:
        FAILURES.append("starter resource node count mismatch")
    return spawned_count


def ensure_map_actors() -> None:
    platform, platform_created = spawn_or_get_actor(
        STARTER_PLATFORM_LABEL,
        "/Script/Ocean.OceanFloatingPlatform",
        unreal.Vector(0.0, 0.0, 60.0),
    )
    desired_core_size = unreal.IntPoint(2, 2)
    current_core_size = get_prop(platform, ["initial_core_size", "InitialCoreSize"])
    core_size_changed = not values_equal(current_core_size, desired_core_size)
    set_prop(platform, ["initial_core_size", "InitialCoreSize"], unreal.IntPoint(2, 2), required=False, label=STARTER_PLATFORM_LABEL)
    if platform_created or core_size_changed:
        call_if_exists(platform, ["initialize_core_platform", "InitializeCorePlatform"])

    resource_field, _resource_field_created = spawn_or_get_actor(
        STARTER_RESOURCE_FIELD_LABEL,
        "/Script/Ocean.OceanResourceField",
        unreal.Vector(0.0, 0.0, 80.0),
    )
    count_ok = set_prop(resource_field, ["fallback_resource_count", "FallbackResourceCount"], FALLBACK_RESOURCE_COUNT, required=False, label=STARTER_RESOURCE_FIELD_LABEL)
    radius_ok = set_prop(resource_field, ["field_radius", "FieldRadius"], RESOURCE_FIELD_RADIUS, required=False, label=STARTER_RESOURCE_FIELD_LABEL)
    exclusion_ok = set_prop(resource_field, ["exclusion_radius", "ExclusionRadius"], RESOURCE_EXCLUSION_RADIUS, required=False, label=STARTER_RESOURCE_FIELD_LABEL)
    if not count_ok:
        count_ok, _ = call_if_exists(resource_field, ["set_fallback_resource_count", "SetFallbackResourceCount"], FALLBACK_RESOURCE_COUNT)
    if not radius_ok:
        radius_ok, _ = call_if_exists(resource_field, ["set_field_radius", "SetFieldRadius"], RESOURCE_FIELD_RADIUS)
    if not exclusion_ok:
        exclusion_ok, _ = call_if_exists(resource_field, ["set_exclusion_radius", "SetExclusionRadius"], RESOURCE_EXCLUSION_RADIUS)

    tdd("MVPResourceFieldConfig", f"fallback_count={FALLBACK_RESOURCE_COUNT} result={pass_fail(count_ok)}", failed=not count_ok)
    tdd("MVPResourceFieldRadius", f"field_radius={RESOURCE_FIELD_RADIUS} result={pass_fail(radius_ok)}", failed=not radius_ok)
    tdd("MVPResourceFieldExclusion", f"exclusion_radius={RESOURCE_EXCLUSION_RADIUS} result={pass_fail(exclusion_ok)}", failed=not exclusion_ok)
    spawn_starter_resource_nodes(resource_field)

    spawn_or_get_actor(
        STARTER_PLAYER_START_LABEL,
        "/Script/Engine.PlayerStart",
        unreal.Vector(-350.0, -450.0, 180.0),
        unreal.Rotator(0.0, 45.0, 0.0),
    )


def save_and_verify() -> None:
    unreal.EditorAssetLibrary.save_directory("/Game/OceanPrototype", only_if_is_dirty=True, recursive=True)
    save_result = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    tdd("MVPSetupSave", f"result={pass_fail(bool(save_result))}", failed=not save_result)

    required_assets = [
        MOVE_ACTION_PATH,
        INTERACT_ACTION_PATH,
        TOGGLE_BUILD_ACTION_PATH,
        ROTATE_BUILD_ACTION_PATH,
        INPUT_CONTEXT_PATH,
        SURVIVOR_BP_PATH,
        GAMEMODE_BP_PATH,
        DECK_DEFINITION_PATH,
    ]
    for asset_path in required_assets:
        exists = unreal.EditorAssetLibrary.does_asset_exist(asset_path)
        tdd("MVPAssetExists", f"path={asset_path} result={pass_fail(exists)}", failed=not exists)

    if not save_result:
        raise RuntimeError("save_dirty_packages returned False")


def main() -> None:
    load_map()
    ensure_input_assets()
    ensure_blueprints_and_data()
    ensure_map_actors()
    save_and_verify()

    if FAILURES:
        raise RuntimeError("MVP setup failed:\n" + "\n".join(FAILURES))


try:
    main()
except Exception:
    unreal.log_error(traceback.format_exc())
    raise
