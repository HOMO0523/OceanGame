---
unit_id: 2026-06-17-paper2d-animation-set
status: implemented
owner: asset-pipeline
updated_at: 2026-06-17T02:16:00
source_commit: pending
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.Paper2DAnimationSet
---

# Paper2D / HD2D 角色动作集 — 实施记录

## 已完成

- 生成并整理 `idle`、`swim`、`climb`、`jump`、`divesuit_dive` 五组新动作。
- 保留已有 `walk` 动作，并纳入总 manifest。
- 所有新动作完成绿幕去除、透明 PNG 输出、`178x222` 单元归一、单帧拆分。
- 生成动作总览预览图：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\ocean_survivor_animation_set_preview.png`
- 根据“idle 和其他动作一样保持 8 帧、所有网格对齐”的要求，生成了 `256x256` 单元的五动作合成 atlas：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\ocean_survivor_actions_5x4dir_8f_atlas_alpha_grid256x256.png`
- 根据“五张图合成一张大的重新生成”的要求，尝试了两版单次大图重生：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\ocean_survivor_actions_5x4dir_8f_regenerated_alpha_grid256x256.png`
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\ocean_survivor_actions_5x4dir_8f_regenerated_v2_alpha_grid256x256.png`
- 根据最终修订方案，重新单独生成五张 `8x4` 动作块，再脚本合成一张最终 `8x20` atlas：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_atlas_alpha_grid256x256.png`
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_audit_preview.png`
- 根据用户反馈“256 切图不对 / 有些头部会被裁掉”，重新从五张绿幕源图反推切图比例：
  - 源图尺寸：`1774x887`
  - 源定位格：`221.75x221.75`，取 `222x222`
  - 推荐 UE 导入安全格：`288x288`
  - 安全 atlas：`D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v4_safe\ocean_survivor_actions_5x4dir_8f_source222_pad33_safe_atlas_alpha_grid288x288.png`
- 2026-06-17: 在 `Ocean` 项目中启用 Paper2D 插件，并把 `Paper2D` 加入 `Ocean.Build.cs` runtime dependency。
- 2026-06-17: 给 `AOceanCharacter` 添加 `Paper2DVisualComponent`，作为表现层挂在 `BP_OceanSurvivorCharacter` 上；Capsule、移动、状态、背包、交互和建造组件继续保留 gameplay 权威。
- 2026-06-17: 使用 `scripts/import_paper2d_experiment.py` 导入实验帧目录 `atlas_source_ratio_222_pad33_v5_walk_safe\frames_alpha_288x288`。
- 2026-06-17: UE 实验导入结果为 192 个 Textures、192 个 Sprites、24 个 Flipbooks，路径为 `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe`。
- 2026-06-17: `BP_OceanSurvivorCharacter` 默认指定 `FB_ocean_survivor_idle_south`，用于验证 Paper2D 组件、Flipbook 可加载和默认显示链路。

## 产物位置

- 动作总 manifest：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\ocean_survivor_animation_set_manifest.json`
- `256x256` 五动作 8 帧 manifest：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\ocean_survivor_actions_8f_grid256_manifest.json`
- 单次大图重生 v1 manifest：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\ocean_survivor_actions_5x4dir_8f_regenerated_manifest.json`
- 单次大图重生 v2 manifest：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\ocean_survivor_actions_5x4dir_8f_regenerated_v2_manifest.json`
- 最终推荐 manifest：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_manifest.json`
- 最终推荐拆帧目录：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\frames_alpha_256x256`
- 当前推荐安全拆帧目录：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v4_safe\frames_alpha_288x288`
- Paper2D 实验拆帧目录：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v5_walk_safe\frames_alpha_288x288`
- Ocean UE 实验导入目录：
  - `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe/Textures`
  - `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe/Sprites`
  - `/Game/OceanPrototype/Paper2D/Experiment/V5WalkSafe/Flipbooks`
- 当前推荐安全 manifest：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v4_safe\ocean_survivor_actions_5x4dir_8f_source222_pad33_safe_manifest.json`
- 动作导出根目录：
  - `D:\UE5 demo\Paper2d\Export\OceanSurvivor`

## 备注

- 原始生产产物仍存放在 `D:\UE5 demo\Paper2d`；本轮只把 V5 walk-safe 实验帧导入 `Ocean` Git 仓库内，用于验证 UE Paper2D 技术链路。
- 当前 UE 导入资产是实验资产，不代表最终 walk 动画质量已经通过；侧向走路腿部相位仍应继续按人工反馈修图。
- 当前不建议直接把单次大图重生 v1/v2 作为 UE 生产导入源；审计发现 160 格一次生成会产生行序错位和贴边裁切。
- 当前推荐使用 `atlas_final_8x20` 目录下的最终合成稿作为 UE Paper2D 导入候选。
- 若在 UE 中使用 Grid 切分，当前优先使用 `atlas_source_ratio_222_pad33_v4_safe`，切图尺寸设为 `288x288`；不要再用 `256x256` 切这批绿幕源图。
- 若只是验证 UE Paper2D 导入、Sprite、Flipbook、Pivot 和状态机接线，可使用 `atlas_source_ratio_222_pad33_v5_walk_safe\frames_alpha_288x288` 作为实验帧目录；该目录不代表最终走路动画质量已通过。
