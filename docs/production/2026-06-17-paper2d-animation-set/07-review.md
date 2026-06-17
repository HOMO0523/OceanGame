---
unit_id: 2026-06-17-paper2d-animation-set
status: reviewed
owner: asset-pipeline
updated_at: 2026-06-17T02:16:00
source_commit: 24a3c4f
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

## 风险

- 生图得到的连续帧不是骨骼动画，局部服装和手脚可能有轻微跳变。
- 绿幕去除可能在发丝和浅色边缘留下轻微半透明边缘。
- 潜水服状态是新外观，必须和后续装备系统 / 事件系统的状态切换保持一致。
- 单次大图重生的 160 格复杂度过高，当前已实测会出现行序错位、动作混入和贴边裁切。

## 下一步

1. 人工挑选是否接受当前视觉方向。
2. Paper2D 技术实验优先使用 `atlas_source_ratio_222_pad33_v5_walk_safe\frames_alpha_288x288`，只验证导入、Sprite、Flipbook、Pivot 和方向映射。
3. 正式美术导入前继续修复 `walk`，尤其是侧向走路的一黑丝腿 / 一裸腿相位连续性。
4. 为每个动作创建 Paper2D flipbook。
5. 在 `BP_OceanSurvivorCharacter` 或未来专用 Paper2D 表现组件里接入状态机。
6. 单独修复 `WASD` 四方向都朝右走的问题，并用日志验证输入向量与动画方向。
