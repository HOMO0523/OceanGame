---
unit_id: 2026-06-17-paper2d-animation-set
status: planned
owner: asset-pipeline
updated_at: 2026-06-17T02:16:00
source_commit: 24a3c4f
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.Paper2DAnimationSet
---

# Paper2D / HD2D 角色动作集 — 测试设计

## 静态文件测试

| 测试 | 预期 |
|---|---|
| 最终 atlas 存在 | `atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_atlas_alpha_grid256x256.png` 存在 |
| 最终 atlas 尺寸 | `2048x5120` |
| 拆帧数量 | `frames_alpha_256x256` 下存在 160 个 PNG |
| 单元尺寸 | 每个动作帧为 `256x256` |
| 动作数量 | 5 个动作：`idle`、`swim`、`climb`、`jump`、`divesuit_dive` |
| 方向数量 | 每个动作 4 行：`south`、`west`、`east`、`north` |
| 帧数量 | 每个动作每个方向 8 帧 |

## 视觉/审计测试

| 测试 | 预期 |
|---|---|
| 空帧检查 | 无空帧 |
| 裁切检查 | 无 `near_clip` 问题 |
| 地面动作基线 | `idle`、`climb`、`jump` 稳定落在同一基线 |
| 中心锚点 | `swim`、`divesuit_dive` 使用中心锚点 |
| 单次大图直生对照 | v1/v2 直生稿保留，但标记为不推荐导入 |

## UE 接入测试（后续）

- 导入最终 atlas 后，使用 Paper2D Grid 切分 `256x256` 单元。
- 创建五个动作的 Flipbook，并验证行序映射到 `S/A/D/W`。
- 在角色表现层切换动作时，确认 Capsule / Ocean gameplay components 不被替换。

