---
name: unreal-bridge
description: Execute Python inside a running UE5.7 editor through the project-local UnrealBridge plugin and scripts. Use for asset creation, save gates, PIE control, log capture, and editor-state inspection.
---

# UnrealBridge Workflow

## When To Use

Use this before asking the user to click in the Unreal Editor when the task involves:

- creating or modifying UE assets,
- saving dirty packages,
- running PIE,
- inspecting maps, Blueprints, PCG graphs, or components,
- capturing `[TDD]` logs,
- validating generated Ocean content.

## Connection

Preferred client:

```powershell
python scripts/ue_tdd_pipeline.py --check-only
```

For custom Python execution, import `BridgeClient`:

```python
from scripts.ue_tdd_bridge import BridgeClient

client = BridgeClient()
assert client.connect()
result = client.send("import unreal\nprint(unreal.SystemLibrary.get_project_directory())")
print(result)
```

The client first tries UDP multicast discovery, then falls back to probing local editor-owned TCP listeners.

## Save Gate

Before closing or restarting the editor:

```python
import unreal
dirty_before = [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()]
dirty_before += [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()]
save_result = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
dirty_after = [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()]
dirty_after += [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()]
print(f"save_result={save_result} dirty_before={dirty_before} dirty_after={dirty_after}")
```

If bridge cannot connect and unsaved editor work may exist, stop and report the blocker. Do not kill the editor.

## Ocean Asset Checks

Useful bridge probes:

- Verify map or asset loadability with `unreal.EditorAssetLibrary.does_asset_exist`.
- Verify floating actors keep `BuoyancyComponent`.
- Verify Water and PCG assets are present in the expected map.
- Verify generated assets are saved after creation.

## No-LiveCoding Rule

UnrealBridge can inspect a running editor, but it is not proof of C++ compile correctness. For C++ verification, run:

```powershell
python scripts/ue_tdd_pipeline.py --pie-duration 5
```
