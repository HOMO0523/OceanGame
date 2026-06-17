---
unit_id: 2026-06-17-paper2d-animation-set
status: verified
owner: asset-pipeline
updated_at: 2026-06-17T02:16:00
source_commit: pending
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.Paper2DAnimationSet
---

# Paper2D / HD2D 角色动作集 — 验证结果

## 文件验证

| 动作 ID | Grid 尺寸 | 帧数 |
|---|---:|---:|
| `idle` | `712x888` | 16 |
| `walk` | `1780x888` | 40 |
| `swim` | `1424x888` | 32 |
| `climb` | `1424x888` | 32 |
| `jump` | `1424x888` | 32 |
| `divesuit_dive` | `1424x888` | 32 |

## 解释

- 所有 grid 高度均为 `4 * 222 = 888`。
- `idle` 宽度为 `4 * 178 = 712`。
- `walk` 宽度为 `10 * 178 = 1780`。
- 其余 8 帧动作宽度为 `8 * 178 = 1424`。

## 视觉检查

- `idle`、`walk`、`swim`、`climb`、`jump`、`divesuit_dive` 均可读。
- `climb` 是占位过渡动作，后续 UE 里需要用交互点精修落点。
- `divesuit_dive` 已使用潜水服外观，但正式美术可替换。

## 8 帧合成检查

| 产物 | 尺寸 | 帧数 | 结论 |
|---|---:|---:|---|
| `ocean_survivor_actions_5x4dir_8f_atlas_alpha_grid256x256.png` | `2048x5120` | 160 | 可作为合成检查稿；由五组动作后处理合成 |
| `ocean_survivor_actions_5x4dir_8f_regenerated_alpha_grid256x256.png` | `2048x5120` | 160 | 不建议导入；`swim` 起始行混入站立背面 |
| `ocean_survivor_actions_5x4dir_8f_regenerated_v2_alpha_grid256x256.png` | `2048x5120` | 160 | 不建议导入；仍有行序错位、部分动作贴边 |
| `atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_atlas_alpha_grid256x256.png` | `2048x5120` | 160 | 推荐导入候选；分动作生成后脚本合成 |
| `atlas_source_ratio_222_pad33_v4_safe\ocean_survivor_actions_5x4dir_8f_source222_pad33_safe_atlas_alpha_grid288x288.png` | `2304x5760` | 160 | 当前推荐导入候选；按五张绿幕图反推比例并加安全边 |

## 单次大图重生边界

- 单次生成 `8x20 = 160` 格时，模型会优先保证“看起来像 sprite sheet”，但无法稳定遵守每一行的动作语义。
- 当前 v1/v2 都已保存，作为对比证据；正式 UE 导入应优先使用“分动作生成高分辨率 → 脚本合成一张总 atlas”的流程。
- 如果必须追求同一张生图的统一画风，需要降低单张复杂度，例如一次只生成一个动作块 `8x4`，再合成为 `8x20`。

## 最终合成稿验证

| 项 | 结果 |
|---|---|
| 生产方式 | 每个动作单独 `8x4` 重生，再脚本合成 `8x20` |
| 最终 atlas | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_atlas_alpha_grid256x256.png` |
| 单帧目录 | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\frames_alpha_256x256` |
| 审计预览 | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_audit_preview.png` |
| Manifest | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_manifest.json` |
| 帧数 | 160 |
| 网格 | `8` 列 x `20` 行，单元 `256x256` |
| 脚本审计问题数 | 0 |

## 最终合成稿比例摘要

| 动作 | 中位高度 | 平均高度 | 中位宽度 | 平均宽度 |
|---|---:|---:|---:|---:|
| `idle` | 224.5 | 226.25 | 81.5 | 80.97 |
| `swim` | 146.0 | 148.44 | 182.0 | 184.84 |
| `climb` | 155.5 | 166.22 | 114.0 | 124.22 |
| `jump` | 189.0 | 194.16 | 94.0 | 102.53 |
| `divesuit_dive` | 176.5 | 187.72 | 173.0 | 171.44 |

说明：`swim`、`divesuit_dive` 是横向游泳姿势，不能用全身高度和站立动作直接比较；本轮用同一全局缩放系数和锚点对齐降低头部 / 身体比例跳变。

## 源图比例与安全切图验证

| 项 | 结果 |
|---|---|
| 五张绿幕源图尺寸 | `1774x887` |
| 自然单元比例 | `221.75x221.75` |
| 源定位格 | `222x222` |
| 安全导入格 | `288x288` |
| 安全边 | 每侧约 `33px`，并自动保证最小边距 `12px` |
| 安全 atlas | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v4_safe\ocean_survivor_actions_5x4dir_8f_source222_pad33_safe_atlas_alpha_grid288x288.png` |
| 安全拆帧 | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v4_safe\frames_alpha_288x288` |
| 安全审计预览 | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v4_safe\ocean_survivor_actions_5x4dir_8f_source222_pad33_safe_audit_preview.png` |
| 安全审计问题数 | 0 |

边界结论：`222x222` 是用于从绿幕源图定位每个动作帧的比例；UE Paper2D 导入应使用安全 atlas 的 `288x288` 网格，避免头发、鞋子、脚蹼贴边被裁掉。

## UE Paper2D 实验导入验证

| 项 | 结果 |
|---|---|
| 插件可用性 | `PaperSprite`、`PaperFlipbook`、`PaperSpriteFactory`、`PaperFlipbookFactory`、`PaperSpriteComponent`、`PaperFlipbookComponent` 均可在 Unreal Python 中解析 |
| 导入源目录 | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_source_ratio_222_pad33_v5_walk_safe\frames_alpha_288x288` |
| 源 PNG 帧数 | 192 |
| 动作 / 方向 / 帧数 | `idle`、`walk`、`swim`、`climb`、`jump`、`divesuit_dive` × 4 方向 × 8 帧 |
| UE Textures | 192 |
| UE Sprites | 192 |
| UE Flipbooks | 24 |
| 默认角色 Flipbook | `FB_ocean_survivor_idle_south` 已赋给 `BP_OceanSurvivorCharacter` 的 `Paper2DVisualComponent` |
| 保存状态 | 导入目录与角色蓝图保存成功 |

## 命令证据

```powershell
python scripts/ue_tdd_pipeline.py --pie-duration 3 --log-lines 160
python scripts/ue_tdd_bridge.py --exec-file scripts/import_paper2d_experiment.py
```
