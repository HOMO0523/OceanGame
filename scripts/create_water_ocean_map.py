"""
Create the first Ocean water map.

Run from Unreal Editor Python, for example:
    UnrealEditor.exe Ocean.uproject -ExecutePythonScript=scripts/create_water_ocean_map.py

Creates or updates:
    /Game/OceanPrototype/Maps/L_WaterOcean
"""

from __future__ import annotations

import traceback
import os

import unreal


MAP_DIR = "/Game/OceanPrototype/Maps"
MAP_PATH = f"{MAP_DIR}/L_WaterOcean"
WATER_ZONE_EXTENT = 250_000.0
OCEAN_COLLISION_Z = 10_000.0


def tdd(name: str, message: str) -> None:
    unreal.log(f"[TDD] {name}: {message}")
    print(f"[TDD] {name}: {message}")


def get_class(script_path: str):
    loaded = unreal.load_class(None, script_path)
    if loaded is None:
        raise RuntimeError(f"Could not load class {script_path}")
    return loaded


def set_prop(obj, prop_name: str, value) -> bool:
    try:
        obj.set_editor_property(prop_name, value)
        return True
    except Exception:
        return False


def call_if_exists(obj, method_name: str, *args) -> bool:
    method = getattr(obj, method_name, None)
    if method is None:
        return False
    try:
        method(*args)
        return True
    except Exception:
        return False


def get_level_subsystem():
    subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if subsystem is None:
        raise RuntimeError("LevelEditorSubsystem is unavailable")
    return subsystem


def create_or_load_level() -> None:
    unreal.EditorAssetLibrary.make_directory(MAP_DIR)
    level_subsystem = get_level_subsystem()

    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        if not level_subsystem.load_level(MAP_PATH):
            raise RuntimeError(f"Failed to load existing level {MAP_PATH}")
        tdd("WaterOceanMapLoaded", "result=PASS")
        return

    if not level_subsystem.new_level(MAP_PATH):
        raise RuntimeError(f"Failed to create new level {MAP_PATH}")
    tdd("WaterOceanMapCreated", "result=PASS")


def find_actor_by_label(label: str):
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        try:
            if actor.get_actor_label() == label:
                return actor
        except Exception:
            continue
    return None


def spawn_or_get_actor(label: str, class_path: str, location: unreal.Vector):
    existing = find_actor_by_label(label)
    if existing is not None:
        return existing

    actor_class = get_class(class_path)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        location,
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    if actor is None:
        raise RuntimeError(f"Failed to spawn {label} from {class_path}")
    actor.set_actor_label(label)
    return actor


def configure_water_zone(water_zone) -> None:
    extent2d = unreal.Vector2D(WATER_ZONE_EXTENT, WATER_ZONE_EXTENT)
    if not call_if_exists(water_zone, "set_zone_extent", extent2d):
        set_prop(water_zone, "zone_extent", extent2d)

    set_prop(water_zone, "render_target_resolution", unreal.IntPoint(1024, 1024))
    call_if_exists(water_zone, "update")


def configure_ocean_body(water_ocean) -> None:
    ocean_component = water_ocean.get_water_body_component()
    if ocean_component is None:
        raise RuntimeError("WaterBodyOcean has no WaterBodyComponent")

    ocean_extent = unreal.Vector2D(WATER_ZONE_EXTENT, WATER_ZONE_EXTENT)
    collision_extent = unreal.Vector(WATER_ZONE_EXTENT, WATER_ZONE_EXTENT, OCEAN_COLLISION_Z)

    if not call_if_exists(ocean_component, "set_collision_extents", collision_extent):
        set_prop(ocean_component, "collision_extents", collision_extent)

    set_prop(ocean_component, "ocean_extents", ocean_extent)
    call_if_exists(ocean_component, "fill_water_zone_with_ocean")


def ensure_landscape_support() -> None:
    library = getattr(unreal, "OceanLandscapeAutomationLibrary", None)
    if library is None:
        tdd("OceanSupportLandscapeLibrary", "result=FAIL")
        raise RuntimeError("OceanLandscapeAutomationLibrary is unavailable; build/load the OceanEditor module")

    report = library.ensure_water_ocean_landscape_support()
    if report:
        print(str(report).strip())


def add_visibility_helpers() -> None:
    spawn_or_get_actor(
        "PlayerStart_WaterOcean",
        "/Script/Engine.PlayerStart",
        unreal.Vector(0.0, 0.0, 250.0),
    )

    light = spawn_or_get_actor(
        "DirectionalLight_WaterOcean",
        "/Script/Engine.DirectionalLight",
        unreal.Vector(-600.0, -600.0, 800.0),
    )
    light.set_actor_rotation(unreal.Rotator(-45.0, -35.0, 0.0), False)

    spawn_or_get_actor(
        "SkyAtmosphere_WaterOcean",
        "/Script/Engine.SkyAtmosphere",
        unreal.Vector(0.0, 0.0, 0.0),
    )

    spawn_or_get_actor(
        "ExponentialHeightFog_WaterOcean",
        "/Script/Engine.ExponentialHeightFog",
        unreal.Vector(0.0, 0.0, 0.0),
    )


def save_and_verify() -> None:
    save_result = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    exists = unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH)
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    water_ocean_count = sum(1 for actor in actors if actor.get_actor_label() == "WaterBodyOcean_Prototype")
    water_zone_count = sum(1 for actor in actors if actor.get_actor_label() == "WaterZone_Prototype")
    landscape_count = sum(1 for actor in actors if actor.get_actor_label() == "Landscape_WaterSupport")
    landscape_component_count = 0
    water_brush_count = 0
    for actor in actors:
        if actor.get_actor_label() == "Landscape_WaterSupport":
            landscape_component_count += len(actor.get_components_by_class(unreal.LandscapeComponent))
        if actor.get_actor_label() == "WaterBrushManager_Prototype":
            water_brush_count += 1

    tdd("WaterOceanSave", f"result={'PASS' if save_result else 'FAIL'}")
    tdd("WaterOceanMapExists", f"result={'PASS' if exists else 'FAIL'}")
    tdd("WaterOceanActorCount", f"actual={water_ocean_count} expected=1")
    tdd("WaterZoneActorCount", f"actual={water_zone_count} expected=1")
    tdd("OceanSupportLandscapeCount", f"actual={landscape_count} expected=1")
    tdd("OceanSupportLandscapeComponentCount", f"actual={landscape_component_count} expected=256")
    tdd("OceanWaterBrushManagerCount", f"actual={water_brush_count} expected=1")

    if not save_result:
        raise RuntimeError("save_dirty_packages returned False")
    if not exists:
        raise RuntimeError(f"Map asset does not exist after save: {MAP_PATH}")
    if water_ocean_count != 1 or water_zone_count != 1:
        raise RuntimeError("WaterOcean map actor counts are not correct")
    if landscape_count != 1 or landscape_component_count != 256 or water_brush_count != 1:
        raise RuntimeError("WaterOcean landscape support counts are not correct")


def main() -> None:
    create_or_load_level()

    water_zone = spawn_or_get_actor(
        "WaterZone_Prototype",
        "/Script/Water.WaterZone",
        unreal.Vector(0.0, 0.0, 0.0),
    )
    configure_water_zone(water_zone)

    water_ocean = spawn_or_get_actor(
        "WaterBodyOcean_Prototype",
        "/Script/Water.WaterBodyOcean",
        unreal.Vector(0.0, 0.0, 0.0),
    )
    configure_ocean_body(water_ocean)
    ensure_landscape_support()

    add_visibility_helpers()
    save_and_verify()

    if os.environ.get("OCEAN_QUIT_AFTER_SCRIPT") == "1":
        unreal.SystemLibrary.quit_editor()


try:
    main()
except Exception:
    unreal.log_error(traceback.format_exc())
    raise
