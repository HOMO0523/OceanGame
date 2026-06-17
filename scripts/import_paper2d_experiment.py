"""
Import the external Ocean survivor Paper2D experiment frames into UE assets.

Run from Unreal Editor Python, or through UnrealBridge:
    UnrealEditor.exe Ocean.uproject -ExecutePythonScript=scripts/import_paper2d_experiment.py
"""

from __future__ import annotations

import re
from pathlib import Path

import unreal


SOURCE_FRAME_DIR = Path(r"D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v5_walk_safe\frames_alpha_288x288")
DEST_ROOT = "/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe"
TEXTURE_DIR = f"{DEST_ROOT}/Textures"
SPRITE_DIR = f"{DEST_ROOT}/Sprites"
FLIPBOOK_DIR = f"{DEST_ROOT}/Flipbooks"
SURVIVOR_BP_PATH = "/Game/OceanPrototype/Blueprints/BP_OceanSurvivorCharacter"

FRAME_RE = re.compile(r"^ocean_survivor_(?P<action>.+)_(?P<direction>south|west|east|north)_(?P<frame>\d{2})\.png$")
EXPECTED_ACTIONS = ("climb", "divesuit_dive", "idle", "jump", "swim", "walk")
EXPECTED_DIRECTIONS = ("south", "west", "east", "north")
EXPECTED_FRAMES_PER_DIRECTION = 8
CELL_SIZE = 288.0
PIXELS_PER_UNREAL_UNIT = 1.5
LAND_ACTIONS = {"idle", "walk", "jump", "climb"}
ACTION_FPS = {
    "idle": 4.0,
    "walk": 8.0,
    "swim": 8.0,
    "climb": 10.0,
    "jump": 10.0,
    "divesuit_dive": 8.0,
}

FAILURES: list[str] = []


def tdd(name: str, message: str, failed: bool = False) -> None:
    line = f"[TDD] {name}: {message}"
    unreal.log(line)
    print(line)
    if failed or "result=FAIL" in message:
        FAILURES.append(line)


def pass_fail(value: bool) -> str:
    return "PASS" if value else "FAIL"


def ensure_dir(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def load_asset(path: str):
    return unreal.EditorAssetLibrary.load_asset(path)


def create_or_load_asset(asset_name: str, package_path: str, asset_class, factory):
    asset_path = f"{package_path}/{asset_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        return load_asset(asset_path)
    return unreal.AssetToolsHelpers.get_asset_tools().create_asset(asset_name, package_path, asset_class, factory)


def parse_frames() -> dict[tuple[str, str], list[tuple[int, Path]]]:
    groups: dict[tuple[str, str], list[tuple[int, Path]]] = {}
    for frame_path in SOURCE_FRAME_DIR.glob("*.png"):
        match = FRAME_RE.match(frame_path.name)
        if not match:
            continue
        action = match.group("action")
        direction = match.group("direction")
        frame_index = int(match.group("frame"))
        groups.setdefault((action, direction), []).append((frame_index, frame_path))

    for key in list(groups):
        groups[key].sort(key=lambda item: item[0])
    return groups


def import_textures(frame_paths: list[Path]) -> dict[str, object]:
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    tasks = []
    for frame_path in frame_paths:
        task = unreal.AssetImportTask()
        task.filename = str(frame_path)
        task.destination_path = TEXTURE_DIR
        task.destination_name = frame_path.stem
        task.automated = True
        task.replace_existing = True
        task.save = False
        tasks.append(task)

    asset_tools.import_asset_tasks(tasks)

    textures = {}
    for frame_path in frame_paths:
        texture = load_asset(f"{TEXTURE_DIR}/{frame_path.stem}")
        if texture:
            configure_texture(texture)
            textures[frame_path.stem] = texture

    return textures


def configure_texture(texture) -> None:
    texture_settings = {
        "mip_gen_settings": ("TextureMipGenSettings", "TMGS_NO_MIPMAPS"),
        "filter": ("TextureFilter", "TF_NEAREST"),
    }
    for prop, enum_info in texture_settings.items():
        enum_name, value_name = enum_info
        enum_type = getattr(unreal, enum_name, None)
        if enum_type is None or not hasattr(enum_type, value_name):
            continue
        try:
            texture.set_editor_property(prop, getattr(enum_type, value_name))
        except Exception:
            pass


def configure_sprite(sprite, texture, action: str) -> None:
    sprite.set_editor_property("source_texture", texture)
    sprite.set_editor_property("source_uv", unreal.Vector2D(0.0, 0.0))
    sprite.set_editor_property("source_dimension", unreal.Vector2D(CELL_SIZE, CELL_SIZE))
    sprite.set_editor_property("source_texture_dimension", unreal.Vector2D(CELL_SIZE, CELL_SIZE))
    sprite.set_editor_property("pixels_per_unreal_unit", PIXELS_PER_UNREAL_UNIT)

    if action in LAND_ACTIONS:
        sprite.set_editor_property("pivot_mode", unreal.SpritePivotMode.BOTTOM_CENTER)
        sprite.set_editor_property("custom_pivot_point", unreal.Vector2D(CELL_SIZE * 0.5, CELL_SIZE))
    else:
        sprite.set_editor_property("pivot_mode", unreal.SpritePivotMode.CENTER_CENTER)
        sprite.set_editor_property("custom_pivot_point", unreal.Vector2D(CELL_SIZE * 0.5, CELL_SIZE * 0.5))


def create_sprites(groups: dict[tuple[str, str], list[tuple[int, Path]]], textures: dict[str, object]) -> dict[str, object]:
    sprites = {}
    for (action, _direction), frames in groups.items():
        for frame_index, frame_path in frames:
            texture = textures.get(frame_path.stem)
            if texture is None:
                continue
            sprite_name = f"SPR_{frame_path.stem}"
            sprite = create_or_load_asset(sprite_name, SPRITE_DIR, unreal.PaperSprite, unreal.PaperSpriteFactory())
            configure_sprite(sprite, texture, action)
            sprites[frame_path.stem] = sprite
    return sprites


def create_flipbooks(groups: dict[tuple[str, str], list[tuple[int, Path]]], sprites: dict[str, object]) -> dict[tuple[str, str], object]:
    flipbooks = {}
    for action in EXPECTED_ACTIONS:
        for direction in EXPECTED_DIRECTIONS:
            frames = groups.get((action, direction), [])
            flipbook_name = f"FB_ocean_survivor_{action}_{direction}"
            flipbook = create_or_load_asset(flipbook_name, FLIPBOOK_DIR, unreal.PaperFlipbook, unreal.PaperFlipbookFactory())
            keyframes = []
            for _frame_index, frame_path in frames:
                sprite = sprites.get(frame_path.stem)
                if sprite is None:
                    continue
                keyframe = unreal.PaperFlipbookKeyFrame()
                keyframe.set_editor_property("sprite", sprite)
                keyframe.set_editor_property("frame_run", 1)
                keyframes.append(keyframe)

            flipbook.set_editor_property("frames_per_second", ACTION_FPS.get(action, 8.0))
            flipbook.set_editor_property("key_frames", keyframes)
            invalidate = getattr(flipbook, "invalidate_cached_data", None)
            if invalidate:
                invalidate()
            flipbooks[(action, direction)] = flipbook
    return flipbooks


def assign_default_flipbook(flipbooks: dict[tuple[str, str], object]) -> bool:
    survivor_bp = load_asset(SURVIVOR_BP_PATH)
    generated_class = survivor_bp.generated_class() if survivor_bp and hasattr(survivor_bp, "generated_class") else None
    if generated_class is None:
        generated_class = getattr(survivor_bp, "GeneratedClass", None)
    if generated_class is None:
        generated_class = unreal.EditorAssetLibrary.find_package_referencers_for_asset(SURVIVOR_BP_PATH, False)

    try:
        survivor_class = survivor_bp.generated_class()
    except Exception:
        survivor_class = None
    if survivor_class is None:
        try:
            survivor_class = survivor_bp.get_editor_property("generated_class")
        except Exception:
            survivor_class = None
    if survivor_class is None:
        return False

    cdo = unreal.get_default_object(survivor_class)
    component_class = getattr(unreal, "PaperFlipbookComponent", None)
    if component_class is None:
        return False
    components = list(cdo.get_components_by_class(component_class))
    if len(components) != 1:
        return False

    idle_south = flipbooks.get(("idle", "south"))
    if idle_south is None:
        return False
    components[0].set_editor_property("source_flipbook", idle_south)
    return True


def main() -> None:
    ensure_dir(DEST_ROOT)
    ensure_dir(TEXTURE_DIR)
    ensure_dir(SPRITE_DIR)
    ensure_dir(FLIPBOOK_DIR)

    paper_classes_ok = all(getattr(unreal, name, None) is not None for name in ("PaperSprite", "PaperFlipbook", "PaperSpriteFactory", "PaperFlipbookFactory"))
    tdd("Paper2DPluginClasses", f"result={pass_fail(paper_classes_ok)}", failed=not paper_classes_ok)

    groups = parse_frames()
    frame_paths = [frame_path for frames in groups.values() for _frame_index, frame_path in frames]
    expected_total = len(EXPECTED_ACTIONS) * len(EXPECTED_DIRECTIONS) * EXPECTED_FRAMES_PER_DIRECTION
    tdd("Paper2DSourceFrameCount", f"actual={len(frame_paths)} expected={expected_total} result={pass_fail(len(frame_paths) == expected_total)}", failed=len(frame_paths) != expected_total)

    for action in EXPECTED_ACTIONS:
        for direction in EXPECTED_DIRECTIONS:
            count = len(groups.get((action, direction), []))
            ok = count == EXPECTED_FRAMES_PER_DIRECTION
            tdd("Paper2DFrameGroup", f"action={action} direction={direction} actual={count} expected={EXPECTED_FRAMES_PER_DIRECTION} result={pass_fail(ok)}", failed=not ok)

    textures = import_textures(frame_paths)
    tdd("Paper2DTextureImportCount", f"actual={len(textures)} expected={expected_total} result={pass_fail(len(textures) == expected_total)}", failed=len(textures) != expected_total)

    sprites = create_sprites(groups, textures)
    tdd("Paper2DSpriteCount", f"actual={len(sprites)} expected={expected_total} result={pass_fail(len(sprites) == expected_total)}", failed=len(sprites) != expected_total)

    flipbooks = create_flipbooks(groups, sprites)
    expected_flipbooks = len(EXPECTED_ACTIONS) * len(EXPECTED_DIRECTIONS)
    tdd("Paper2DFlipbookCount", f"actual={len(flipbooks)} expected={expected_flipbooks} result={pass_fail(len(flipbooks) == expected_flipbooks)}", failed=len(flipbooks) != expected_flipbooks)

    assigned = assign_default_flipbook(flipbooks)
    tdd("Paper2DDefaultFlipbookAssigned", f"path={SURVIVOR_BP_PATH} flipbook=FB_ocean_survivor_idle_south result={pass_fail(assigned)}", failed=not assigned)

    save_result = unreal.EditorAssetLibrary.save_directory(DEST_ROOT)
    save_result = bool(save_result) and bool(unreal.EditorAssetLibrary.save_loaded_asset(load_asset(SURVIVOR_BP_PATH)))
    tdd("Paper2DSave", f"result={pass_fail(bool(save_result))}", failed=not save_result)

    if FAILURES:
        raise RuntimeError("Paper2D import failed:\n" + "\n".join(FAILURES))


if __name__ == "__main__":
    main()
