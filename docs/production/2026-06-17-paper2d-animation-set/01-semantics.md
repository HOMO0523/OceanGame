---
unit_id: 2026-06-17-paper2d-animation-set
status: frozen
owner: asset-pipeline
updated_at: 2026-06-17T02:16:00
source_commit: 24a3c4f
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.Paper2DAnimationSet
---

# Paper2D / HD2D 角色动作集 — 语义冻结

unit_type: ui
name: Paper2D / HD2D survivor animation atlas
accepted_target: five action blocks generated as 8x4 sheets and composed into one aligned 8x20 atlas
screen_set: Paper2D survivor visual-state flipbook source, not an in-game menu screen
accepted_asset_paths: `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\ocean_survivor_actions_5x4dir_8f_final_atlas_alpha_grid256x256.png`, `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20\frames_alpha_256x256`
required_controls: `S/A/D/W` direction rows map to `south/west/east/north`; animation state switches are driven by gameplay state, not by UI buttons
data_bindings: current movement direction, surface state, climb transition state, jump transition state, diving suit state
command_dispatch: no direct command dispatch in this asset unit; future Paper2D component reads gameplay state from `BP_OceanSurvivorCharacter`
visual_acceptance: all five actions have four directions and eight frames per direction, cells are 256x256, no empty frames, no script-detected crop or anchor issue
automation_probe: PNG dimension probe, 160-frame count probe, manifest audit with zero detected crop/anchor issues
fallback_policy: if a single 8x20 generation scrambles rows, regenerate each action as 8x4 and script-compose the final atlas
tests_required: static file probes for atlas size, frame count, row/action order, and visual audit preview before UE import

## 动作行序

所有四方向贴图统一使用同一行序，避免 UE 导入后方向映射混乱：

| 行 | 方向 | 输入 |
|---|---|---|
| 1 | `south` / 朝屏幕下方 / 朝镜头 | `S` |
| 2 | `west` / 朝屏幕左方 | `A` |
| 3 | `east` / 朝屏幕右方 | `D` |
| 4 | `north` / 朝屏幕上方 / 背向镜头 | `W` |

## 动作集合

| 动作 ID | 中文含义 | 每方向帧数 | 循环 | 推荐帧率 | 用途 |
|---|---|---:|---|---:|---|
| `idle` | 四方向待机 | 8 | 是 | 4 FPS | 没有移动输入时播放 |
| `swim` | 水面四方向游泳 | 8 | 是 | 8 FPS | 角色在水面但未潜水 |
| `climb` | 游泳上岸 / 爬船 | 8 | 否 | 10 FPS | 水面到船体或岸边的过渡 |
| `jump` | 四方向跳跃 | 8 | 否 | 10 FPS | 小循环内的短跳或地形跨越占位 |
| `divesuit_dive` | 潜水服四方向潜水 | 8 | 是 | 8 FPS | 后续海底探索状态 |

说明：已有 `walk` 占位动作保留为独立旧产物；本次最终 atlas 只覆盖用户要求重生的五个动作。

## 状态边界

- `idle/walk/jump` 属于可站立表面：小船、漂浮平台、岛岸、邮轮甲板。
- `swim` 属于水面状态：角色还在海面上，通常不允许建造、背包拖拽可保留但交互范围缩小。
- `climb` 是过渡动作，不是循环动作；播放期间应锁定移动输入，动画结束后由交互点把角色吸附到船体或岸边合法位置。
- `divesuit_dive` 是潜水状态动作，不等同于普通水面游泳；后续应绑定氧气 / 体力 / 下潜事件。
- 视觉朝向由最近有效移动向量或交互方向决定；不要让鼠标点地移动删除，后续只是在已有点击移动外增加 WASD 输入。

## 当前取舍

- 本轮贴图是 Codex 生图 + 去绿幕得到的第一版占位资产，不作为最终商业级动画。
- 上岸 / 爬船动作不内嵌完整船体或岛岸模型，避免和实际场景资产绑定；UE 里由交互点、碰撞体和动画通知完成吸附。
- 所有动作统一归一到 `178x222` 单元，和已有 walk 资源保持同一 Paper2D 导入规格。
- 潜水服采用“保留角色识别度 + 增加面罩、氧气背包、脚蹼”的独立外观，后续可替换为正式潜水服设定。

## 修订后的资产验收目标

- 最终导入候选不使用一次性 `8x20` 单图直生稿，而使用“每个动作单独 `8x4` 重生，再脚本合成 `8x20`”。
- `idle` 必须和其他动作一样保持每方向 8 帧。
- 最终总 atlas 必须为 `8` 列、`20` 行，每格 `256x256`，总尺寸 `2048x5120`。
- 五个动作块顺序固定为 `idle`、`swim`、`climb`、`jump`、`divesuit_dive`。
- 行序继续固定为 `south`、`west`、`east`、`north`。
- 合成脚本必须使用同一个全局缩放系数，避免动作间头部和身体比例跳变。
- `idle`、`climb`、`jump` 使用稳定地面基线；`swim`、`divesuit_dive` 使用中心锚点。
