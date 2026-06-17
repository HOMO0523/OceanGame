---
unit_id: 2026-06-17-paper2d-animation-set
status: reviewed
owner: asset-pipeline
updated_at: 2026-06-17T02:16:00
source_commit: pending
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.Paper2DAnimationSet
---

# Paper2D / HD2D 角色动作集 — 复盘

## 通过项

- 行序统一，后续导入 UE 后可以按同一方向枚举驱动。
- 帧尺寸统一，方便 Paper2D Grid 切分和批量创建 flipbook。
- 水面游泳、爬船、跳跃、潜水服潜水已经覆盖小循环和后续扩展。
- 最终推荐稿采用分动作 `8x4` 重生再合成，规避了单次 `8x20` 大图直生的行序错位问题。
- `idle` 已补齐为每方向 8 帧，并纳入最终 `8x20` 总 atlas。
- 已从五张绿幕源图反推出真实比例：源定位格 `222x222`，UE 安全导入格 `288x288`。
- 安全版 atlas 的脚本审计问题数为 0，解决了 `256x256` 切图导致的比例错误和头部裁切风险。
- Paper2D 插件已在 Ocean 项目中启用，并通过 `Ocean.Build.cs` 接入 runtime module。
- `BP_OceanSurvivorCharacter` 已挂载 Paper2D Flipbook 表现层，未替换原有 Gameplay Pawn。
- V5 walk-safe 实验帧已导入 UE，形成 192 Textures、192 Sprites、24 Flipbooks，可用于后续方向/动作状态机实验。

## 风险

- 生图得到的连续帧不是骨骼动画，局部服装和手脚可能有轻微跳变。
- 绿幕去除可能在发丝和浅色边缘留下轻微半透明边缘。
- 潜水服状态是新外观，必须和后续装备系统 / 事件系统的状态切换保持一致。
- 单次大图重生的 160 格复杂度过高，当前已实测会出现行序错位、动作混入和贴边裁切。
- 当前 UE 导入的是技术实验资产，不能等同于最终美术验收；侧向 `walk` 腿部相位仍需按“一黑丝腿 / 一裸腿”连续性继续修。
- Paper2D 表现层不能接管移动、碰撞、交互或建造逻辑，否则会破坏 `BP_OceanSurvivorCharacter` 的 MVP gameplay 组件边界。

## 下一步

1. 正式美术导入前继续修复 `walk`，尤其是侧向走路的一黑丝腿 / 一裸腿相位连续性。
2. 接入 Paper2D 状态机：按移动方向、跳跃、水面/陆地、潜水入口状态切换 flipbook。
3. 保持 `BP_OceanSurvivorCharacter` 为唯一 Gameplay Pawn，Paper2D 只读状态并负责显示。
4. 后续把 `swim`、`climb`、`divesuit_dive` 接到真实事件系统前，先保留为表现资源，不提前承诺完整潜水玩法。
